from fastapi import FastAPI, File, UploadFile
from fastapi.responses import JSONResponse
from fastapi.middleware.cors import CORSMiddleware
from fastapi.staticfiles import StaticFiles
import subprocess
import tempfile
import os
import json


app = FastAPI(title="Line of Sight API")
app.mount("/", StaticFiles(directory="static", html=True), name="static")

# Global path to latest uploaded CSV
ELEVATION_FILE = None

# Enable CORS
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


@app.post("/upload")
async def upload_file(file: UploadFile = File(...)):
    global ELEVATION_FILE
    # Save to a temporary file
    with tempfile.NamedTemporaryFile(delete=False, suffix=".csv") as tmp:
        tmp.write(await file.read())
        ELEVATION_FILE = tmp.name
    return {"status": "ok", "elevation_file": ELEVATION_FILE}

@app.post("/los")
async def compute_los(data: dict):
    global ELEVATION_FILE
    if not ELEVATION_FILE:
        return JSONResponse(status_code=400, content={"error": "No elevation file uploaded yet."})
    
    p1, p2 = data["p1"], data["p2"]

    # Build command
    cmd = [
        "../solver/bin/los",  # path to your compiled program
        "-f", ELEVATION_FILE,
        "-p1", str(p1["lat"]), str(p1["lon"]), str(p1["height_m"]),
        "-p2", str(p2["lat"]), str(p2["lon"]), str(p2["height_m"]),
        "-o", "json"
    ]

    # Run process
    result = subprocess.run(cmd, capture_output=True, text=True)

    if result.returncode != 0:
        return JSONResponse(status_code=500, content={"error": result.stderr})

    # Parse JSON output from your program
    try:
        return json.loads(result.stdout)
    except json.JSONDecodeError:
        return JSONResponse(status_code=500, content={"error": "Invalid output from LOS program."})
