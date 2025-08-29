#!/bin/bash
set -e

# Backend solver
cd solver
make
cd ..

# Frontend
cd gui
npm install
npm run build
cd ..

# Server
cd server
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
deactivate
cd ..