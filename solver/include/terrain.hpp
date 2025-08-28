#pragma once

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>

#define SAMPLES_STEPS 100 // Number of samples along the line of sight. Must be >= 2

/**
 * 
 * @brief Terrain elevation grid and related functions
 * 
 */

namespace terrain {

class ElevationGrid {
public:
    ElevationGrid() = default;
    
    ElevationGrid(const std::vector<double>& lats,
                  const std::vector<double>& lngs,
                  const std::vector<double>& alts);

    // Build grid from CSV file: lat,lng,alt
    static ElevationGrid fromCSV(const std::string& filepath);

    // Interpolation
    double bilinearInterpolation(double lat, double lng) const;

    // Line of sight between two (lat,lng) points
    bool lineOfSight(double lat1, double lng1,
                     double lat2, double lng2,
                     double observerHeight = 2.0,
                     double targetHeight   = 2.0) const;

private:
    std::vector<double> latitudes;
    std::vector<double> longitudes;
    std::vector<std::vector<double>> elevationGrid;

    int findIndex(const std::vector<double>& vec, double value) const;
};

} // namespace terrain
