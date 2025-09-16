# Veradynium

<img src="gui/logo/logo.png" width="100" />

## LoRaWAN gateways placement problem: solver and network analysis

This project provides a set of programs and utilities to design and analyze a LoRaWAN deployments with energy and terrain elevation (topography) considerations. The [solver](solver) contains a set of tools to compute the optimal placement of the gateways based on end-devices locations and the terrain elevation map, allowing the user to choose between solutions that prioritizes energy consumption or number of gateways to place.

A Python [API](server) is provided to expose the set of solver programs and access them via HTTP. It also allows to upload terrain elevation files (.csv or .nc) and network configuration (.geojson).

The [GUI](GUI) was implemented with React.js, MUI and Leaflet.js, between other libraries.

## Installation
Run ```build.sh``` or ```make all``` to set up the project.  

## Build step by step
To build the project step by step, follow these instructions:

### Binaries
Compile executables  
```bash
cd solver
make
```
#### Tests
Check if two points are in Line-Of-Sight (LOS) given the elevation map  
```bash
los -f elevation.csv -p1 36.733780 -91.237743 2.0 -p2 36.712818 -91.221097 2.5
```
Test end-devices to gateways allocation given elevation map and nodes location:
```bash
compute_allocation -f elevation.csv -g network.json -o json
```
Run the solver to compute the optimal placement of gateways
```bash
solver -f elevation.csv -g network.json -o json  
```

### GUI
Build GUI
```bash
cd gui
npm install
npm run dev # Optional, to run development version
npm run build # Target is ../server/static
```

### Server
Install and start the server for the API  
```bash
cd server
virtualenv venv
source venv/bin/activate
pip3 install -r requirements.txt
uvicorn main:app --reload 
deactivate
```
The GUI will be available at ```localhost:8080``` (if previously compiled).