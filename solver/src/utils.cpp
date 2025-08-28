#include "../include/utils.hpp"

namespace utils {
// ------------------- Print Help ------------------- //
void printHelp(const char* file, const char* message) { // Open readme file with manual and print on terminal   
    std::cerr << std::endl << message << std::endl << std::endl;
    std::ifstream manualFile(file);
    if (manualFile.is_open()) {
        std::string line;
        while (getline(manualFile, line)) {
            std::cout << line << std::endl;
        }
        manualFile.close();
        exit(1);
    } else {
        std::cerr << "Error: Unable to open manual file." << std::endl;
    }
}

} // namespace utils