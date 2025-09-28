#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>

#include "global.hpp"

#define SAMPLES_STEPS 100 // Number of samples along the line of sight. Must be >= 2

/**
 * 
 * @brief Terrain elevation grid and related functions
 * 
 */

namespace terrain {

struct LatLngAlt {
    double lat = 0.0;
    double lng = 0.0;
    double alt = 0.0;

    // Operators for vector arithmetic (ignoring altitude changes)
    LatLngAlt operator+(const LatLngAlt& other) const {
        return { lat + other.lat, lng + other.lng, alt};
    }

    LatLngAlt& operator+=(const LatLngAlt& other) {
        lat += other.lat;
        lng += other.lng;
        return *this;
    }

    LatLngAlt operator-(const LatLngAlt& other) const {
        return { lat - other.lat, lng - other.lng, alt};
    }

    LatLngAlt operator-=(const LatLngAlt& other) {
        lat -= other.lat;
        lng -= other.lng;
        return *this;
    }

    LatLngAlt operator*(double scalar) const {
        return { lat * scalar, lng * scalar, alt};
    }

    LatLngAlt operator/(double scalar) const {
        return { lat / scalar, lng / scalar, alt};
    }
};

class FeatureCollection {
public:
    FeatureCollection() = default;
    std::vector<LatLngAlt> points;
};

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

    // Terrain profile between two (lat,lng) points with given number of steps
    void terrainProfile(double lat1, double lng1,
                        double lat2, double lng2,
                        std::vector<double>& profile,
                        std::vector<double>& distances,
                        int steps = SAMPLES_STEPS) const;

    // Line of sight between two (lat,lng) points
    bool lineOfSight(double lat1, double lng1,
                     double lat2, double lng2,
                     double observerHeight = 2.0,
                     double targetHeight   = 2.0) const;
    bool lineOfSight(LatLngAlt pos1, LatLngAlt pos2) const;

    // Haversine distance between two lat/lng points in meters
    double haversine(double lat1, double lon1, double lat2, double lon2) const;
    double haversine(LatLngAlt pos1, LatLngAlt pos2) const;

    // Compute straight line distance between two lat/lng/alt points in meters
    double distance(double lat1, double lon1, 
                     double lat2, double lon2,
                     double h1, double h2) const; 
    double distance(LatLngAlt pos1, LatLngAlt pos2) const;

    // Check if a lat/lng is within the grid bounds
    inline bool inElevationGrid(double lat, double lng) const {
        return !(lat < latitudes.front() || lat > latitudes.back() ||
                 lng < longitudes.front() || lng > longitudes.back());
    }
    inline bool inElevationGrid(LatLngAlt pos) const {
        return inElevationGrid(pos.lat, pos.lng);
    }

    inline std::vector<LatLngAlt> getBoundingBox() const {
        return {
            {latitudes.front(), longitudes.front(), 0.0},
            {latitudes.front(), longitudes.back(),  0.0},
            {latitudes.back(),  longitudes.back(),  0.0},
            {latitudes.back(),  longitudes.front(), 0.0}
        };
    }

    double getMaxAltitude() const;
    double getMinAltitude() const;

private:
    std::vector<double> latitudes;
    std::vector<double> longitudes;
    std::vector<std::vector<double>> elevationGrid;

    int findIndex(const std::vector<double>& vec, double value) const;
};

inline int clampIndexForCell(int n, int idx) {
    // valid cell index range is [0, n-2] because we access [idx] and [idx+1]
    if (n < 2) return -1;
    if (idx < 0) return 0;
    if (idx > n - 2) return n - 2;
    return idx;
}

// WGS-84 constants
constexpr double a  = 6378137.0;           // semi-major axis
constexpr double f  = 1.0 / 298.257223563; // flattening
constexpr double e2 = f * (2 - f);         // eccentricity^2

struct Vec3 { double x, y, z; };

// Convert geodetic coordinates to ECEF (Earth-Centered, Earth-Fixed)
Vec3 toECEF(double lat, double lng, double h);

LatLngAlt getCentroid(const std::vector<LatLngAlt>& points); // Points must have lat and lng members

} // namespace terrain