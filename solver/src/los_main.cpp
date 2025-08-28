#define MANUAL "assets/los_manual.txt"

#include <iostream>
#include <cstring>
#include "../include/utils.hpp"
#include "../include/terrain.hpp"

int main(int argc, char **argv) {

    std::string filename;

    double lat1 = 0.0, lon1 = 0.0, h1 = 2.0;
    double lat2 = 0.0, lon2 = 0.0, h2 = 2.0;

    for(int i = 0; i < argc; i++) {    
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 || argc == 1)
            utils::printHelp(MANUAL);

        if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) {
            if(i+1 < argc) {
                const char* file = argv[i+1];
                filename = std::string(file);
            }else{
                utils::printHelp(MANUAL, "Error in argument -f (--file)");
            }
        }

        if (strcmp(argv[i], "-p1") == 0) {
            if (i + 2 < argc) { // must have at least 2 more args
                lat1 = atof(argv[++i]);
                lon1 = atof(argv[++i]);

                // Optional altitude: only parse if present and not another option
                if (i + 1 < argc && argv[i+1][0] != '-') {
                    h1 = atof(argv[++i]);
                }else{
                    std::cout << "Using default observer height h1 = 2.0m\n";
                }
            } else {
                utils::printHelp(MANUAL, "Error: -p1 requires at least 2 values (lat lon) and optional altitude");
            }
        }

        if( strcmp(argv[i], "-p2") == 0) {
            if (i + 2 < argc) { // must have at least 2 more args
                lat2 = atof(argv[++i]);
                lon2 = atof(argv[++i]);

                // Optional altitude: only parse if present and not another option
                if (i + 1 < argc && argv[i+1][0] != '-') {
                    h2 = atof(argv[++i]);
                }else{
                    std::cout << "Using default target height h2 = 2.0m\n";
                }
            } else {
                utils::printHelp(MANUAL, "Error: -p2 requires at least 2 values (lat lon) and optional altitude");
            }
        }
    }

    if(filename.empty()){
        utils::printHelp(MANUAL, "Error in argument -f (--file)");
    }

    // Validate coordinates
    if (lat1 < -90.0 || lat1 > 90.0 || lat2 < -90.0 || lat2 > 90.0 ||
        lon1 < -180.0 || lon1 > 180.0 || lon2 < -180.0 || lon2 > 180.0) {
        utils::printHelp(MANUAL, "Latitude must be in [-90, 90] and Longitude in [-180, 180]");
        return 1;
    }
    if (h1 < 0.0 || h2 < 0.0) {
        utils::printHelp(MANUAL, "Heights must be non-negative");
        return 1;
    }

    auto grid = terrain::ElevationGrid::fromCSV(filename);

    if (grid.lineOfSight(lat1, lon1, lat2, lon2, h1, h2)) {
        std::cout << "Clear line of sight\n";
    } else {
        std::cout << "Obstructed\n";
    }

    return 0;
}

