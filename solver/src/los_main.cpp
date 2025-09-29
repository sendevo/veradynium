#define MANUAL "assets/los_manual.txt"

#include <iostream>
#include <cstring>
#include "../include/global.hpp"
#include "../include/terrain.hpp"

int main(int argc, char **argv) {

    std::string filename;

    global::PRINT_TYPE outputFormat = global::PLAIN_TEXT;

    double lat1 = 0.0, lon1 = 0.0, h1 = 2.0;
    double lat2 = 0.0, lon2 = 0.0, h2 = 2.0;

    for(int i = 0; i < argc; i++) {    
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 || argc == 1)
            global::printHelp(MANUAL);

        if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) {
            if(i+1 < argc) {
                const char* file = argv[i+1];
                filename = std::string(file);
            }else{
                global::printHelp(MANUAL, "Error in argument -f (--file). A filename must be provided");
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
                global::printHelp(MANUAL, "Error in argument -p1. At least 2 values (lat lng) and optional altitude must be provided");
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
                global::printHelp(MANUAL, "Error in argument -p2. At least 2 values (lat lng) and optional altitude must be provided");
            }
        }

        if(strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if(i+1 < argc) {
                const char* fmt = argv[i+1];
                if(strcmp(fmt, "text") == 0) {
                    outputFormat = global::PLAIN_TEXT;
                } else if(strcmp(fmt, "json") == 0) {
                    outputFormat = global::JSON;
                } else {
                    global::printHelp(MANUAL, "Error in argument -o (--output). Supported formats: text, json");
                }
            } else {
                global::printHelp(MANUAL, "Error in argument -o (--output)");
            }
        }

        if(strcmp(argv[i], "--dbg") == 0) {
            global::dbg.rdbuf(std::cout.rdbuf()); // Enable debug output to std::cout
        }
    }

    if(filename.empty()){
        global::printHelp(MANUAL, "Error in argument -f (--file). A filename must be provided.");
    }

    // Validate coordinates
    if (lat1 < -90.0 || lat1 > 90.0 || lat2 < -90.0 || lat2 > 90.0 ||
        lon1 < -180.0 || lon1 > 180.0 || lon2 < -180.0 || lon2 > 180.0) {
        global::printHelp(MANUAL, "Latitude must be in [-90, 90] and Longitude in [-180, 180]");
        return 1;
    }
    if (h1 < 0.0 || h2 < 0.0) {
        global::printHelp(MANUAL, "Heights must be non-negative");
        return 1;
    }

    auto grid = terrain::ElevationGrid::fromCSV(filename);

    if (!grid.inElevationGrid(lat1, lon1)) {
        global::printHelp(MANUAL, "Point 1 is outside the elevation grid bounds");
        return 1;
    }
    if (!grid.inElevationGrid(lat2, lon2)) {
        global::printHelp(MANUAL, "Point 2 is outside the elevation grid bounds");
        return 1;
    }

    const bool los = grid.lineOfSight(lat1, lon1, lat2, lon2, h1, h2);
    const bool losFresnel = grid.lineOfSight(lat1, lon1, lat2, lon2, h1, h2, true);

    const double totalDistance = grid.equirectangularDistance(lat1, lon1, lat2, lon2);

    std::vector<double> profile;
    std::vector<double> distances;
    grid.terrainProfile(lat1, lon1, lat2, lon2, profile, distances);

    switch(outputFormat) {
        case global::PLAIN_TEXT:
            std::cout << "Line of sight from (" 
                << lat1 << ", " << lon1 << ", " << h1 << "m) to (" 
                << lat2 << ", " << lon2 << ", " << h2 << "m), ";
            std::cout << "distance of " << totalDistance << " m: ";
            if (los) std::cout << "CLEAR\n"; else std::cout << "BLOCKED\n";
            if (los != losFresnel) {
                std::cout << "  (Note: with Fresnel zone clearance, the link is ";
                if (losFresnel) std::cout << "CLEAR\n"; else std::cout << "BLOCKED\n";
                std::cout << "  assuming 60% Fresnel zone clearance)\n";
            }
            break;
        case global::JSON:
            std::cout << "{\n"
                << "  \"point1\": {\"lat\": " << lat1 << ", \"lng\": " << lon1 << ", \"height_m\": " << h1 << "},\n"
                << "  \"point2\": {\"lat\": " << lat2 << ", \"lng\": " << lon2 << ", \"height_m\": " << h2 << "},\n"
                << "  \"distance_m\": " << totalDistance << ",\n"
                << "  \"line_of_sight\": " << (los ? "true" : "false") << ",\n"
                << "  \"line_of_sight_fresnel_60pct\": " << (losFresnel ? "true" : "false") << ",\n";
                
                std::cout << "  \"terrain_profile_elev_m\": [";
                    for (size_t i = 0; i < profile.size(); ++i) {
                        std::cout << profile[i];
                        if (i < profile.size() - 1) 
                            std::cout << ", ";
                    }
                    std::cout << "],\n";
                
                std::cout << "  \"terrain_profile_dist_m\": [";
                for (size_t i = 0; i < distances.size(); ++i) {
                    std::cout << distances[i];
                    if (i < distances.size() - 1) 
                        std::cout << ", ";
                }
                std::cout << "]\n";
            std::cout << "}\n";
            break;
        default:
            break;
    }

    return 0;
}

