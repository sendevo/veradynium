from fastapi import FastAPI, File, UploadFile, APIRouter
from fastapi.responses import JSONResponse, FileResponse
from fastapi.staticfiles import StaticFiles
from fastapi.middleware.cors import CORSMiddleware
import subprocess, tempfile, json
import os

app = FastAPI(title="Line of Sight API")

# Frontend build
app.mount("/static", StaticFiles(directory="static"), name="static")

@app.get("/")
async def root():
    return FileResponse("static/index.html")

# API under /api
api = APIRouter(prefix="/api")

ELEVATION_FILE = None

@api.post("/upload")
async def upload_file(file: UploadFile = File(...)):
    global ELEVATION_FILE
    with tempfile.NamedTemporaryFile(delete=False, suffix=".csv") as tmp:
        tmp.write(await file.read())
        ELEVATION_FILE = tmp.name
    print(f"Uploaded file saved to {ELEVATION_FILE}")
    return {"status": "ok", "elevation_file": ELEVATION_FILE}

@api.post("/los")
async def compute_los(data: dict):
    global ELEVATION_FILE
    if not ELEVATION_FILE:
        return JSONResponse(status_code=400, content={"error": "No elevation file uploaded yet."})
    p1, p2 = data["p1"], data["p2"]
    cmd = [
        "../solver/bin/los",
        "-f", ELEVATION_FILE,
        "-p1", str(p1["lat"]), str(p1["lon"]), str(p1["height_m"]),
        "-p2", str(p2["lat"]), str(p2["lon"]), str(p2["height_m"]),
        "-o", "json"
    ]
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        return JSONResponse(status_code=500, content={"error": result.stderr})
    try:
        print("LOS command output:")
        print(result.stdout)
        return json.loads(result.stdout)
    except json.JSONDecodeError:
        return JSONResponse(status_code=500, content={"error": "Invalid output from LOS program."})

    os.remove(ELEVATION_FILE)
    ELEVATION_FILE = None

app.include_router(api)

# CORS if you ever run the frontend on a different origin
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)
