#define MANUAL "manual.txt"

#include <iostream>
#include <cstring>
#include "constants.cpp"

int main(int argc, char **argv) {

    for(int i = 0; i < argc; i++) {    
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 || argc == 1)
            utils::printHelp(MANUAL);
        if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) {
            if(i+1 < argc){
                const char* file = argv[i+1];
                // Read and parse json file with model data
            }else{
                utils::printHelp(MANUAL);
                std::cerr << std::endl << "Error in argument -f (--file)" << std::endl;
            }
        }
    }

    return 0;
}

