#define MANUAL "assets/los_manual.txt"

#include <iostream>
#include <cstring>
#include "../include/utils.hpp"
#include "../include/terrain.hpp"

enum OUTPUT_FORMAT { PLAIN_TEXT, JSON };

int main(int argc, char **argv) {

    std::string filename;

    OUTPUT_FORMAT outputFormat = PLAIN_TEXT;

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
                utils::printHelp(MANUAL, "Error in argument -f (--file). A filename must be provided");
            }
        }

        if (strcmp(argv[i], "-p1") == 0) {
            if (i + 2 < argc) { // must have at least 2 more args
                lat1 = atof(argv[++i]);
                lon1 = atof(argv[++i]);

                // Optional altitude: only parse if present and not another option
                if (i + 1 < argc && argv[i+1][0] != '-') {
                    h1 = atof(argv[++i]);
                }
            } else {
                utils::printHelp(MANUAL, "Error in argument -p1. At least 2 values (lat lon) and optional altitude must be provided");
            }
        }

        if( strcmp(argv[i], "-p2") == 0) {
            if (i + 2 < argc) { // must have at least 2 more args
                lat2 = atof(argv[++i]);
                lon2 = atof(argv[++i]);

                // Optional altitude: only parse if present and not another option
                if (i + 1 < argc && argv[i+1][0] != '-') {
                    h2 = atof(argv[++i]);
                }
            } else {
                utils::printHelp(MANUAL, "Error in argument -p2. At least 2 values (lat lon) and optional altitude must be provided");
            }
        }

        if(strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if(i+1 < argc) {
                const char* fmt = argv[i+1];
                if(strcmp(fmt, "text") == 0) {
                    outputFormat = PLAIN_TEXT;
                } else if(strcmp(fmt, "json") == 0) {
                    outputFormat = JSON;
                } else {
                    utils::printHelp(MANUAL, "Error in argument -o (--output). Supported formats: text, json");
                }
            } else {
                utils::printHelp(MANUAL, "Error in argument -o (--output)");
            }
        }
    }

    if(filename.empty()){
        utils::printHelp(MANUAL, "Error in argument -f (--file). A filename must be provided.");
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

    const bool los = grid.lineOfSight(lat1, lon1, lat2, lon2, h1, h2);
    const double distance = grid.distance(lat1, lon1, lat2, lon2, h1, h2);

    switch(outputFormat) {
        case PLAIN_TEXT:
            std::cout << "Line of sight from (" 
                << lat1 << ", " << lon1 << ", " << h1 << "m) to (" 
                << lat2 << ", " << lon2 << ", " << h2 << "m), ";
            std::cout << "distance of " << distance << " m: ";
            if (los) std::cout << "CLEAR\n"; else std::cout << "BLOCKED\n";
            break;
        case JSON:
            std::cout << "{\n"
                      << "  \"point1\": {\"lat\": " << lat1 << ", \"lon\": " << lon1 << ", \"height_m\": " << h1 << "},\n"
                      << "  \"point2\": {\"lat\": " << lat2 << ", \"lon\": " << lon2 << ", \"height_m\": " << h2 << "},\n"
                      << "  \"distance_m\": " << distance << ",\n"
                      << "  \"line_of_sight\": \"" << (los ? "clear" : "blocked") << "\"\n"
                      << "}\n";
            break;
        default:
            break;
    }

    return 0;
}

