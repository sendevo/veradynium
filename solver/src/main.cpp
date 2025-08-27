#define MANUAL "manual.txt"

#include <iostream>
#include <cstring>
#include "../include/constants.hpp"
#include "../include/terrain.hpp"

int main(int argc, char **argv) {

    std::string filename;

    for(int i = 0; i < argc; i++) {    
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 || argc == 1)
            utils::printHelp(MANUAL);
        if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) {
            if(i+1 < argc){
                const char* file = argv[i+1];
                filename = std::string(file);
                // Read and parse json file with model data
            }else{
                utils::printHelp(MANUAL);
                std::cerr << std::endl << "Error in argument -f (--file)" << std::endl;
                exit(1);
            }
        }
    }

    auto grid = terrain::ElevationGrid::fromCSV(filename);

    double lat1=-45.825412, lon1=-67.458747; // Escollera
    double lat2=-45.825497, lon2=-67.459707; // Playa km 4
    double lat3=-45.829159, lon3=-67.543146; // Laprida


    if (grid.lineOfSight(lat1, lon1, lat3, lon3)) {
        std::cout << "Clear line of sight\n";
    } else {
        std::cout << "Obstructed\n";
    }

    return 0;
}

