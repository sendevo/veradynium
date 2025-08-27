#pragma once

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>

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
                     double observerHeight = 1.7,
                     double targetHeight   = 1.7) const;

private:
    std::vector<double> latitudes;
    std::vector<double> longitudes;
    std::vector<std::vector<double>> elevationGrid;

    int findIndex(const std::vector<double>& vec, double value) const;
};

} // namespace terrain
