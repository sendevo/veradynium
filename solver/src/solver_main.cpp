#define MANUAL "assets/solver_manual.txt"

#include <iostream>
#include <cstring>

#include "../include/json.hpp"
#include "../include/global.hpp"
#include "../include/terrain.hpp"
#include "../include/network.hpp"
#include "../include/optimizers.hpp"


int main(int argc, char **argv) {

    std::string em_filename; // Terrain elevation model file (csv)
    std::string nw_filename; // Network file (geojson)
    int max_iterations = 500; // Max iterations for the optimizers
    double min_speed = 1e-6;   // Convergence threshold
    double acceleration = 0.001; // Acceleration factor for the optimizers

    global::PRINT_TYPE outputFormat = global::PLAIN_TEXT;

    for(int i = 0; i < argc; i++) {    
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 || argc == 1)
            global::printHelp(MANUAL);

        if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--em_file") == 0) {
            if(i+1 < argc) {
                const char* file = argv[i+1];
                em_filename = std::string(file);
            }else{
                global::printHelp(MANUAL, "Error in argument -f (--file). A filename must be provided");
            }
        }

        if(strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--nw_file") == 0) {
            if(i+1 < argc) {
                const char* file = argv[i+1];
                nw_filename = std::string(file);
            }else{
                global::printHelp(MANUAL, "Error in argument -g (--nw_file). A filename must be provided");
            }
        }

        if(strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--iters") == 0) {
            if(i+1 < argc) {
                const char* file = argv[i+1];
                max_iterations = atoi(file);
            }else{
                global::printHelp(MANUAL, "Error in argument -i (--iters). An integer number must be provided");
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

    /// Load network
    if(nw_filename.empty()) {
        global::printHelp(MANUAL, "Error in argument -g (--nw_file). A filename must be provided.");
    }
    
    /// Load elevation model
    if(em_filename.empty()) {
        global::printHelp(MANUAL, "Error in argument -f (--em_file). A filename must be provided.");
    }
    
    auto grid = terrain::ElevationGrid::fromCSV(em_filename);
    
    auto network = network::Network::fromGeoJSON(nw_filename);
    network.setElevationGrid(grid);

    optimizers::ClusteringOptimizer(network).optimize(max_iterations, min_speed, acceleration);

    //network.print(outputFormat);

    return 0;
}

