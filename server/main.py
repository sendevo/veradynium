from fastapi import FastAPI, File, Form, UploadFile, APIRouter, HTTPException
from fastapi.responses import JSONResponse, FileResponse
from fastapi.staticfiles import StaticFiles
from fastapi.middleware.cors import CORSMiddleware
import os, subprocess, tempfile, json, uuid

app = FastAPI(title="Line of Sight API")

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
    extension = os.path.splitext(file.filename)[1].lower()
    if extension not in [".csv", ".json"]:
        raise HTTPException(status_code=400, detail="Invalid file type. Only CSV and JSON are allowed.")
    
    upload_id = str(uuid.uuid4())
    os.makedirs(upload_dir, exist_ok=True)

    with open(f"{upload_dir}/{upload_id}{extension}", "wb") as f:
        f.write(await file.read())
    
    #uploaded_files[upload_id] = f"{upload_dir}/{upload_id}{extension}"
    print(f"File uploaded: {upload_id}")
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

    result = subprocess.run(cmd, capture_output=True, text=True)

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

    result = subprocess.run(cmd, capture_output=True, text=True)

    if result.returncode != 0:
        return JSONResponse(status_code=500, content={"error": result.stderr})

    try: # Parse JSON output
        print("Solver command output:")
        print(result.stdout)
        return json.loads(result.stdout)
    except json.JSONDecodeError:
        return JSONResponse(status_code=500, content={"error": "Invalid output from Solver program."})
        

app.include_router(api)

# CORS if you ever run the frontend on a different origin
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)
