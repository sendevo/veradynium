#include "../include/utils.hpp"

namespace utils {

std::string getExecutableDir() {
#ifdef _WIN32
    char result[MAX_PATH];
    GetModuleFileName(NULL, result, MAX_PATH);
    std::string path(result);
    return path.substr(0, path.find_last_of("\\/"));
#else
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count == -1) throw std::runtime_error("Cannot resolve /proc/self/exe");
    std::string path(result, count);
    return path.substr(0, path.find_last_of('/'));
#endif
}


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
    } else { // try to load from executable dir
        std::string execDir = getExecutableDir();
        std::string fullPath = execDir + "/" + file;
        std::ifstream defaultManualFile(fullPath);
        if (defaultManualFile.is_open()) {
            std::string line;
            while (getline(defaultManualFile, line)) {
                std::cout << line << std::endl;
            }
            defaultManualFile.close();
            exit(1);
        }
    }

    std::cerr << "Error: Unable to open manual file." << std::endl;
}

} // namespace utils