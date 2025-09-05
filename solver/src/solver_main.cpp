#define MANUAL "assets/solver_manual.txt"

#include <iostream>
#include <cstring>

#include "../include/json.hpp"
#include "../include/utils.hpp"
#include "../include/terrain.hpp"
#include "../include/network.hpp"

enum OUTPUT_FORMAT { PLAIN_TEXT, JSON };

int main(int argc, char **argv) {

    std::string em_filename; // Terrain elevation model file (csv)
    std::string nw_filename; // Network file (geojson)

    OUTPUT_FORMAT outputFormat = PLAIN_TEXT;

    for(int i = 0; i < argc; i++) {    
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 || argc == 1)
            utils::printHelp(MANUAL);

        if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--em_file") == 0) {
            if(i+1 < argc) {
                const char* file = argv[i+1];
                em_filename = std::string(file);
            }else{
                utils::printHelp(MANUAL, "Error in argument -f (--file). A filename must be provided");
            }
        }

        if(strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--nw_file") == 0) {
            if(i+1 < argc) {
                const char* file = argv[i+1];
                nw_filename = std::string(file);
            }else{
                utils::printHelp(MANUAL, "Error in argument -g (--nw_file). A filename must be provided");
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

    /// Load network
    if(nw_filename.empty()) {
        utils::printHelp(MANUAL, "Error in argument -g (--nw_file). A filename must be provided.");
    }
    
    /// Load elevation model
    if(em_filename.empty()) {
        utils::printHelp(MANUAL, "Error in argument -f (--em_file). A filename must be provided.");
    }
    
    auto grid = terrain::ElevationGrid::fromCSV(em_filename);
    auto network = network::Network::fromJSON(nw_filename, grid);

    network.printInfo();

    network.printDistanceMatrix();

    switch(outputFormat) {
        case PLAIN_TEXT:
            std::cout << "Solver not implemented yet." << std::endl;
            break;
        case JSON:
            std::cout << "Solver not implemented yet." << std::endl;
            break;
        default:
            break;
    }

    return 0;
}

