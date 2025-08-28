#pragma once

#include <iostream>
#include <fstream>
#include <cmath>

/**
 * 
 * @brief Utility functions and specification constants
 * 
 */

namespace utils { // Utility functions

    inline constexpr const char defaultMessage[] = "Error in command line arguments. See manual or documentation.";

    // Print help message from file
    void printHelp(const char* file, const char* message = defaultMessage); 

    // Convert degrees to radians
    inline double toRadians(double degree) { return degree * M_PI / 180.0; }
}