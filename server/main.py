import os, subprocess, tempfile, json, uuid
import asyncio
from fastapi import FastAPI, File, Form, UploadFile, APIRouter, HTTPException
from fastapi.responses import JSONResponse, FileResponse
from fastapi.staticfiles import StaticFiles
from fastapi.middleware.cors import CORSMiddleware
from pathlib import Path
from util import nc_to_csv



app = FastAPI(title="Veradynium API")

# Frontend build
app.mount("/static", StaticFiles(directory="static"), name="static")

# Temporary storage for uploaded files (maps file_id -> path)
#uploaded_files = {}
upload_dir = "uploads"

@app.get("/")
async def root():
    return FileResponse("static/index.html")

# API under /api
api = APIRouter(prefix="/api")


def get_uploaded_file(upload_id: str, expected_ext: str):
    # Retrieve the uploaded file path by its ID
    if not os.path.exists(upload_dir):
        raise HTTPException(status_code=404, detail="Directory not found")

    file_path = os.path.join(upload_dir, upload_id) + expected_ext

    print(f"Retrieving file: {file_path}")
    
    if not os.path.exists(file_path):
        detail = "Terrain elevation map not found. Please upload a CSV file." if expected_ext == ".csv" else "GeoJSON features file not found. Please upload a JSON file." 
        raise HTTPException(status_code=404, detail=detail)
    
    if file_path.endswith(expected_ext):
        return file_path
    else:
        raise HTTPException(status_code=400, detail=f"Expected a {expected_ext} file")


@api.post("/upload")
async def upload_file(file: UploadFile = File(...)):
    # Accept only CSV and JSON files
    extension = Path(file.filename).suffix.lower()
    if extension not in [".csv", ".json", ".nc"]:
        raise HTTPException(status_code=400, detail="Invalid file type. Only CSV, JSON or NC are allowed.")
    
    upload_id = str(uuid.uuid4())
    os.makedirs(upload_dir, exist_ok=True)

    original_path = Path(upload_dir) / f"{upload_id}{extension}"
    with open(original_path, "wb") as f:
        f.write(await file.read())

    # If file is .nc (NetCDF DEM), convert to .csv
    if extension == ".nc":
        csv_path = Path(upload_dir) / f"{upload_id}.csv"
        loop = asyncio.get_event_loop()
        data_array = await loop.run_in_executor(
            None,
            lambda: nc_to_csv(
                str(original_path),
                str(csv_path),
                grid_size=(600, 600)
            )
        )
        os.remove(original_path)
        return {"upload_id": upload_id, "extension": ".csv", "data": data_array}
    
    return {"upload_id": upload_id, "extension": extension}
    

@api.post("/delete")
async def delete_file(data: dict):
    upload_id = data.get("upload_id")
    extension = data.get("extension")

    if extension not in [".csv", ".json"]:
        raise HTTPException(status_code=400, detail="Invalid extension. Only CSV and JSON are allowed.")

    file_path = os.path.join(upload_dir, upload_id) + extension
    if os.path.exists(file_path):
        os.remove(file_path)
        print(f"File deleted: {file_path}")
        return {"detail": "File deleted successfully."}
    else:
        raise HTTPException(status_code=404, detail="File not found.")



@api.post("/los")
async def compute_los(data: dict):
    # Data should contain:
    # - em_file_id: ID of the uploaded elevation map CSV file
    # - p1: {lat, lng, height_m}
    # - p2: {lat, lng, height_m}

    em_file_id = data.get("em_file_id") # Elevation map file ID
    em_file_path = get_uploaded_file(em_file_id, ".csv")

    p1, p2 = data["p1"], data["p2"]

    cmd = [
        "../solver/bin/los",
        "-f", em_file_path,
        "-p1", str(p1["lat"]), str(p1["lng"]), str(p1["height_m"]),
        "-p2", str(p2["lat"]), str(p2["lng"]), str(p2["height_m"]),
        "-o", "json"
    ]

    result = subprocess.run(
        cmd, 
        capture_output=True, 
        text=True,
        timeout=10
    )

    if result.returncode != 0:
        return JSONResponse(status_code=500, content={"error": result.stderr})

    try: # Parse JSON output
        print("LOS command output:")
        print(result.stdout)
        return json.loads(result.stdout)
    except json.JSONDecodeError:
        return JSONResponse(status_code=500, content={"error": "Invalid output from LOS program."})


@app.post("/solve")
async def solve(data: dict):
    em_file_id = data.get("em_file_id") # Elevation map file ID
    geojson_file_id = data.get("features_file_id") # GeoJSON features file ID

    em_file_path = get_uploaded_file(em_file_id, ".csv")
    geojson_file_path = get_uploaded_file(geojson_file_id, ".json")

    cmd = [
        "../solver/bin/solver",
        "-f", em_file_path,
        "-g", geojson_file_path,
        "-o", "json"
    ]

    result = subprocess.run(
        cmd, 
        capture_output=True, 
        text=True,
        timeout=60
    )

    if result.returncode != 0:
        return JSONResponse(status_code=500, content={"error": result.stderr})

    try: # Parse JSON output
        print("Solver command output:")
        print(result.stdout)
        return json.loads(result.stdout)
    except json.JSONDecodeError:
        return JSONResponse(status_code=500, content={"error": "Invalid output from Solver program."})


@app.post("/eval")
async def eval_network(data: dict):
    em_file_id = data.get("em_file_id") # Elevation map file ID
    geojson_file_id = data.get("features_file_id") # GeoJSON features file ID

    em_file_path = get_uploaded_file(em_file_id, ".csv")
    geojson_file_path = get_uploaded_file(geojson_file_id, ".json")

    cmd = [
        "../solver/bin/eval_network",
        "-f", em_file_path,
        "-g", geojson_file_path,
        "-o", "json"
    ]

    result = subprocess.run(
        cmd, 
        capture_output=True, 
        text=True,
        timeout=10
    )

    if result.returncode != 0:
        return JSONResponse(status_code=500, content={"error": result.stderr})

    try: # Parse JSON output
        print("Network evaluation command output:")
        print(result.stdout)
        return json.loads(result.stdout)
    except json.JSONDecodeError:
        return JSONResponse(status_code=500, content={"error": "Invalid output from Network Evaluation program."})


app.include_router(api)

# CORS if you ever run the frontend on a different origin
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)
