#define MANUAL "assets/solver_manual.txt"

#include <iostream>
#include <cstring>

#include "../include/json.hpp"
#include "../include/utils.hpp"
#include "../include/terrain.hpp"
#include "../include/network.hpp"

enum OUTPUT_FORMAT { PLAIN_TEXT, JSON };

int main(int argc, char **argv) {

    std::string em_filename;
    std::string nw_filename;

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

    if(em_filename.empty()) {
        utils::printHelp(MANUAL, "Error in argument -f (--em_file). A filename must be provided.");
    }

    if(nw_filename.empty()) {
        utils::printHelp(MANUAL, "Error in argument -g (--nw_file). A filename must be provided.");
    }

    auto grid = terrain::ElevationGrid::fromCSV(em_filename);
    auto network = network::Network::fromJSON(nw_filename);

    switch(outputFormat) {
        case PLAIN_TEXT:
            std::cout << "Plain text output not implemented yet." << std::endl;
            break;
        case JSON:
            // Print network info
            std::cout << "{\n";
            std::cout << "  \"gateways\": [\n";
            for(size_t i = 0; i < network.gateways.size(); i++) {
                const auto& gw = network.gateways[i];
                std::cout << "    {\n";
                std::cout << "      \"id\": \"" << gw.id << "\",\n";
                std::cout << "      \"lat\": " << gw.lat << ",\n";
                std::cout << "      \"lng\": " << gw.lng << ",\n";
                std::cout << "      \"height\": " << gw.height << "\n";
                std::cout << "    }";
                if(i < network.gateways.size() - 1) std::cout << ",";
                std::cout << "\n";
            }
            std::cout << "  ],\n";
            std::cout << "  \"end_devices\": [\n";
            for(size_t i = 0; i < network.end_devices.size(); i++) {
                const auto& ed = network.end_devices[i];
                std::cout << "    {\n";
                std::cout << "      \"id\": \"" << ed.id << "\",\n";
                std::cout << "      \"lat\": " << ed.lat << ",\n";
                std::cout << "      \"lng\": " << ed.lng << ",\n";
                std::cout << "      \"height\": " << ed.height << "\n";
                std::cout << "    }";
                if(i < network.end_devices.size() - 1) std::cout << ",";
                std::cout << "\n";
            }
            std::cout << "  ]\n";
            std::cout << "}\n";
            break;
        default:
            break;
    }

    return 0;
}

