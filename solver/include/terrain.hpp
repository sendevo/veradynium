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
#define EARTH_RADIUS 6371000.0 // in meters
#define US915_LORA_LAMBDA 0.327642031 // in meters (for 915 MHz)
#define EU860_LORA_LAMBDA 0.345383016 // in meters (for 868 MHz)
#define FRESNEL_CLEARANCE_FACTOR 0.6 // 60% clearance
// WGS-84 constants
#define SEMI_MAJOR_AXIS 6378137.0 // semi-major axis
#define EARTH_ECCENTRICITY 0.00669437999 // eccentricity^2

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

struct Vec3 { double x, y, z; };

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
                     double targetHeight   = 2.0,
                     bool fresnelClearance = false) const;
    bool lineOfSight(const LatLngAlt pos1, const LatLngAlt pos2, bool fresnelClearance = false) const;

    // Haversine distance between two lat/lng points in meters
    double haversineDistance(double lat1, double lng1, double lat2, double lng2) const;
    double haversineDistance(const LatLngAlt pos1, const LatLngAlt pos2) const;

    // Compute straight line distance between two lat/lng/alt points in meters
    double straightLineDistance(double lat1, double lng1, 
                     double lat2, double lng2,
                     double h1, double h2) const; 
    double straightLineDistance(const LatLngAlt pos1, const LatLngAlt pos2) const;

    // Squared distance between two lat/lng points (for comparison purposes)
    double squaredDistance(double lat1, double lng1, double lat2, double lng2) const;
    double squaredDistance(const LatLngAlt pos1, const LatLngAlt pos2) const;

    // Equirectangular approximation distance between two lat/lng points in meters
    double equirectangularDistance(double lat1, double lng1, double lat2, double lng2) const;
    double equirectangularDistance(const LatLngAlt pos1, const LatLngAlt pos2) const;

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

    inline size_t getNumLatitudes() const { return latitudes.size(); };
    inline size_t getNumLongitudes() const { return longitudes.size(); };

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

// Convert geodetic coordinates to ECEF (Earth-Centered, Earth-Fixed)
Vec3 toECEF(double lat, double lng, double h);

LatLngAlt getCentroid(const std::vector<LatLngAlt>& points); // Points must have lat and lng members

} // namespace terrain