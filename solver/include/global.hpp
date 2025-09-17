#pragma once

#include <iostream>
#include <fstream>
#include <cmath>
#include <unistd.h>
#include <limits.h>
#include <sstream>
#include <iomanip>
#include <string>
#include <random>

#define INF std::numeric_limits<double>::infinity()

/**
 * 
 * @brief Utility functions and specification constants
 * 
 */



namespace global { // Utility functions

enum PRINT_TYPE { PLAIN_TEXT, JSON };

inline constexpr const char defaultMessage[] = "Error in command line arguments. See manual or documentation.";

// Get directory of the executable (to load the manual file if not specified)
std::string getExecutableDir();

// Generate a simple UUID (not RFC4122 compliant, just for unique IDs)
std::string generate_uuid();

// Print help message from file
void printHelp(const char* file, const char* message = defaultMessage); 

// Convert degrees to radians
inline double toRadians(double degree) { return degree * M_PI / 180.0; }


struct NullBuffer : std::streambuf {
    int overflow(int c) override { return c; }
};

// Debug output stream (disabled by default)
// Usage: dbg << "Debug info: " << value << std::endl;
inline NullBuffer null_buffer;
inline std::ostream null_stream(&null_buffer);
inline std::ostream& dbg = null_stream;

} // namespace global