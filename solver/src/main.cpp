#define MANUAL "manual.txt"

#include <iostream>
#include <cstring>
#include "constants.cpp"
#include "los.cpp"


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

    auto points = readCSV(filename);

    std::vector<double> unique_lats, unique_lngs;
    std::vector<std::vector<double>> elev_grid;
    buildGrid(points, unique_lats, unique_lngs, elev_grid);

    // Example: line of sight between two points
    double lat1=-45.825412, lon1=-67.458747; // esollera
    double lat2=-45.825497, lon2=-67.459707; // playa km 4
    double h1=2.0, h2=2.0; // observer/target height

    if(lineOfSight(lat1, lon1, lat2, lon2, h1, h2, unique_lats, unique_lngs, elev_grid)) {
        std::cout << "Line of sight is clear.\n";
    } else {
        std::cout << "Line of sight is blocked by terrain.\n";
    }

    return 0;
}

