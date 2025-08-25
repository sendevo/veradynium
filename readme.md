# Veradynium

### LoRaWAN gateways placement problem solver

This project contains a set of programs and utilities to design a real-time LoRaWAN network with energy and area topography considerations. The [solver](solver) program can compute a set of optimal placement of the gateways based on end-devices locations and the terrain elevation map, allowing the user to choose between solutions that prioritizes energy consumption or number of gateways to deploy.

The [python](python) folder contains scripts dedicated to the data analysis of the physical network to get insights about the real system.

A React.js based [GUI](gui) is available to interact with the simulator.