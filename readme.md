# Veradynium

## LoRaWAN gateways placement problem solver and network analysis

This project contains a set of programs and utilities to design a real-time LoRaWAN network with energy and area topography considerations. The [solver](solver) program can compute a set of optimal placement of the gateways based on end-devices locations and the terrain elevation map, allowing the user to choose between solutions that prioritizes energy consumption or number of gateways to deploy.

## Installation
To build everything you can run ```build.sh``` or ```make all```.  
Then, start the server with:  
```bash
cd server
virtualenv venv
source venv/bin/activate
uvicorn main:app --reload 
deactivate
```  
The GUI will be available at ```localhost:8080```.

## Build step by step
If you want to build the project 

### Binaries
Compile executables  
```bash
cd solver
make
```
Test LOS between coordinates  
```bash
los -f elevation.csv -p1 36.733780 -91.237743 2.0 -p2 36.712818 -91.221097 2.5
```

Test the Solver 
```bash
solver -i instance.dat
```

### GUI
Build GUI
```bash
cd gui
npm install
npm run dev # Optional, test the GUI
npm run build # Target is ../server/static
```

### Server
Install and start server  
```bash
cd server
virtualenv venv
source venv/bin/activate
pip3 install -r requirements.txt
uvicorn main:app --reload 
deactivate
```