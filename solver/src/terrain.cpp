#include "../include/terrain.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace terrain {

ElevationGrid::ElevationGrid(const std::vector<double>& lats_raw, const std::vector<double>& lngs_raw, const std::vector<double>& alts_raw) {
    if (lats_raw.size() != lngs_raw.size() || lats_raw.size() != alts_raw.size()) {
        throw std::invalid_argument("Latitude, Longitude, and Altitude vectors must be of the same size");
    }
    if (lats_raw.empty()) {
        throw std::invalid_argument("Empty elevation dataset");
    }

    // Build sorted unique axes (copies, do NOT sort the raw arrays)
    latitudes   = lats_raw;
    longitudes  = lngs_raw;
    std::sort(latitudes.begin(),  latitudes.end());
    std::sort(longitudes.begin(), longitudes.end());
    latitudes.erase( std::unique(latitudes.begin(),  latitudes.end()),  latitudes.end() );
    longitudes.erase(std::unique(longitudes.begin(), longitudes.end()), longitudes.end());

    if (latitudes.size() < 2 || longitudes.size() < 2) {
        std::cerr << "Grid must be at least 2x2 for bilinear interpolation" << std::endl;
        exit(1);
    }

    // Initialize grid with NaNs (useful when there are gaps)
    elevationGrid.assign(latitudes.size(), std::vector<double>(longitudes.size(), std::numeric_limits<double>::quiet_NaN()));

    // Fill grid: for each raw point, find its (i,j) on the unique axes
    for (size_t k = 0; k < alts_raw.size(); ++k) {
        const double la = lats_raw[k];
        const double lo = lngs_raw[k];

        auto it_i = std::lower_bound(latitudes.begin(),  latitudes.end(),  la);
        auto it_j = std::lower_bound(longitudes.begin(), longitudes.end(), lo);

        // If exact match not found and value is beyond last element, place at last cell
        int i = int(it_i - latitudes.begin());
        int j = int(it_j - longitudes.begin());
        // We want indices of the *cell*, not the upper bound; shift back one step unless already 0
        i = clampIndexForCell(int(latitudes.size()),  (i > 0 ? i - 1 : i));
        j = clampIndexForCell(int(longitudes.size()), (j > 0 ? j - 1 : j));
        if (i < 0 || j < 0) continue; // can't place (too small grid), but we checked earlier

        elevationGrid[i][j] = alts_raw[k];
    }
};

ElevationGrid ElevationGrid::fromCSV(const std::string& filepath) {
    std::vector<double> lats, lngs, alts;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open CSV file: " + filepath << std::endl;
        exit(1);
    }

    std::string line;
    // Try to skip header if present (simple heuristic)
    if (std::getline(file, line)) {
        if (line.find_first_not_of("0123456789-+., eE") != std::string::npos) {
            // looks like a header; do nothing (we already consumed it)
        } else {
            // data line; process it
            std::stringstream ss(line);
            std::string tok;
            if (std::getline(ss, tok, ',')) { lats.push_back(std::stod(tok)); }
            if (std::getline(ss, tok, ',')) { lngs.push_back(std::stod(tok)); }
            if (std::getline(ss, tok, ',')) { alts.push_back(std::stod(tok)); }
        }
    }

    while (std::getline(file, line)) {
        if (line.empty()){ continue; }
        std::stringstream ss(line);
        std::string tok;
        double la, lo, al;
        if (!std::getline(ss, tok, ',')){ continue; } la = std::stod(tok);
        if (!std::getline(ss, tok, ',')){ continue; } lo = std::stod(tok);
        if (!std::getline(ss, tok, ',')){ continue; } al = std::stod(tok);
        lats.push_back(la);
        lngs.push_back(lo);
        alts.push_back(al);
    }

    return ElevationGrid(lats, lngs, alts);
};

int ElevationGrid::findIndex(const std::vector<double>& arr, double value) const {
    // Returns index i such that arr[i] <= value <= arr[i+1], clamped to valid cell range.
    auto it = std::lower_bound(arr.begin(), arr.end(), value);
    int idx = int(it - arr.begin());
    // Convert to cell index (left neighbor), then clamp
    idx = (idx > 0 ? idx - 1 : idx);
    return clampIndexForCell(int(arr.size()), idx);
};

double ElevationGrid::bilinearInterpolation(double lat, double lng) const {
    int i = findIndex(latitudes,  lat);
    int j = findIndex(longitudes, lng);
    if (i < 0 || j < 0){ 
        std::cerr << "Point out of bounds or grid too small" << std::endl;
        exit(1);
    }

    // Neighboring axis values
    const double y1 = latitudes[i],    y2 = latitudes[i+1];
    const double x1 = longitudes[j],   x2 = longitudes[j+1];

    // Grid cell values
    const double Q11 = elevationGrid[i][j];
    const double Q21 = elevationGrid[i][j+1];
    const double Q12 = elevationGrid[i+1][j];
    const double Q22 = elevationGrid[i+1][j+1];

    // If any is NaN (hole), you could fallback to nearest neighbor:
    auto isnan = [](double v){ return std::isnan(v); };
    if (isnan(Q11) || isnan(Q21) || isnan(Q12) || isnan(Q22)) {
        // nearest neighbor fallback
        double wy = (std::fabs(lat - y1) <= std::fabs(y2 - lat)) ? y1 : y2;
        double wx = (std::fabs(lng - x1) <= std::fabs(x2 - lng)) ? x1 : x2;
        int ii = (wy == y1 ? i : i+1);
        int jj = (wx == x1 ? j : j+1);
        return elevationGrid[ii][jj];
    }

    // Bilinear
    const double tx = (x2 == x1) ? 0.0 : (lng - x1) / (x2 - x1);
    const double ty = (y2 == y1) ? 0.0 : (lat - y1) / (y2 - y1);

    const double fxy1 = Q11 * (1 - tx) + Q21 * tx;
    const double fxy2 = Q12 * (1 - tx) + Q22 * tx;
    return fxy1 * (1 - ty) + fxy2 * ty;
};

void ElevationGrid::terrainProfile(double lat1, double lng1, 
                                   double lat2, double lon2, 
                                   std::vector<double>& profile,
                                   std::vector<double>& distances,
                                   int steps) const {
    profile.reserve(steps + 1);
    distances.reserve(steps + 1);

    const double latDiff = lat2 - lat1;
    const double lonDiff = lon2 - lng1;
    for (int k = 0; k <= steps; ++k) {
        const double t   = double(k) / steps;
        // Next position
        const double lat = lat1 + t * latDiff;
        const double lng = lng1 + t * lonDiff;
        // Compute and add elevation at current position
        const double terrain = bilinearInterpolation(lat, lng);
        profile.push_back(terrain);
        // Compute and add accumulated distance
        const double dist = haversineDistance(lat1, lng1, lat, lng);
        distances.push_back(dist);
    }
};

bool ElevationGrid::lineOfSight(double lat1, double lng1,
                                double lat2, double lon2,
                                double observerHeight,
                                double targetHeight,
                                bool fresnelClearance) const
{
    const double elev1 = bilinearInterpolation(lat1, lng1) + observerHeight;
    const double elev2 = bilinearInterpolation(lat2, lon2) + targetHeight;

    double totalDistance;
    double clearance = 0.0;
    if(fresnelClearance) // only compute if needed
        totalDistance = equirectangularDistance(lat1, lng1, lat2, lon2);

    for (int k = 1; k < SAMPLES_STEPS; ++k) {
        const double t   = double(k) / SAMPLES_STEPS;
        const double lat = lat1 + t * (lat2 - lat1);
        const double lng = lng1 + t * (lon2 - lng1);

        const double terrain = bilinearInterpolation(lat, lng);
        const double los     = elev1 + t * (elev2 - elev1);

        if(fresnelClearance) {
            const double d1 = totalDistance * t;
            const double d2 = totalDistance * (1.0 - t);
            const double r1 = std::sqrt(US915_LORA_LAMBDA * d1 * d2 / (d1 + d2));
            clearance = FRESNEL_CLEARANCE_FACTOR * r1;
        }

        if (terrain > los - clearance) return false; // blocked
    }
    return true; // clear
};

bool ElevationGrid::lineOfSight(LatLngAlt pos1, LatLngAlt pos2, bool fesnelClearance) const {
    return lineOfSight(pos1.lat, pos1.lng, pos2.lat, pos2.lng, pos1.alt, pos2.alt, fesnelClearance);
};

double ElevationGrid::haversineDistance(double lat1, double lng1, double lat2, double lon2) const {
    const double dlat = global::toRadians(lat2 - lat1);
    const double dlon = global::toRadians(lon2 - lng1);
    const double a = std::sin(dlat/2) * std::sin(dlat/2) +
                     std::cos(global::toRadians(lat1)) * std::cos(global::toRadians(lat2)) *
                     std::sin(dlon/2) * std::sin(dlon/2);
    const double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1-a));
    return EARTH_RADIUS * c;
};

double ElevationGrid::haversineDistance(LatLngAlt pos1, LatLngAlt pos2) const {
    return haversineDistance(pos1.lat, pos1.lng, pos2.lat, pos2.lng);
};

Vec3 toECEF(double lat, double lng, double h) {
    double phi = global::toRadians(lat);
    double lambda = global::toRadians(lng);
    double sinphi = std::sin(phi);
    double cosphi = std::cos(phi);
    double sinlambda = std::sin(lambda);
    double coslambda = std::cos(lambda);

    double N = SEMI_MAJOR_AXIS / sqrt(1 - EARTH_ECCENTRICITY * sinphi * sinphi);

    double x = (N + h) * cosphi * coslambda;
    double y = (N + h) * cosphi * sinlambda;
    double z = (N * (1 - EARTH_ECCENTRICITY) + h) * sinphi;

    return {x, y, z};
};

double ElevationGrid::straightLineDistance(double lat1, double lng1, 
                               double lat2, double lon2, 
                               double h1, double h2) const  {
    Vec3 p1 = toECEF(lat1, lng1, h1);
    Vec3 p2 = toECEF(lat2, lon2, h2);
    double dx = p2.x - p1.x, dy = p2.y - p1.y, dz = p2.z - p1.z;
    return std::sqrt(dx*dx + dy*dy + dz*dz);
};

double ElevationGrid::straightLineDistance(LatLngAlt pos1, LatLngAlt pos2) const {
    return straightLineDistance(pos1.lat, pos1.lng, pos2.lat, pos2.lng, pos1.alt, pos2.alt);
};

double ElevationGrid::equirectangularDistance(double lat1, double lng1, double lat2, double lon2) const {
    const double x = global::toRadians(lon2 - lng1) * std::cos(global::toRadians((lat1 + lat2) / 2));
    const double y = global::toRadians(lat2 - lat1);
    return EARTH_RADIUS * std::sqrt(x*x + y*y);
};

double ElevationGrid::equirectangularDistance(LatLngAlt pos1, LatLngAlt pos2) const {
    return equirectangularDistance(pos1.lat, pos1.lng, pos2.lat, pos2.lng);
};

double ElevationGrid::getMaxAltitude() const {
    double maxAlt = -INF;
    for(const auto& row : elevationGrid) {
        double rowMax = *std::max_element(row.begin(), row.end());
        if(rowMax > maxAlt) {
            maxAlt = rowMax;
        }
    }
    return maxAlt;
};

double ElevationGrid::getMinAltitude() const {
    double minAlt = INF;
    for(const auto& row : elevationGrid) {
        double rowMin = *std::min_element(row.begin(), row.end());
        if(rowMin < minAlt) {
            minAlt = rowMin;
        }
    }
    return minAlt;
};

LatLngAlt getCentroid(const std::vector<LatLngAlt>& points) {
    if(points.empty()) 
        return {0.0, 0.0, 0.0};

    double sumLat{}, sumLng{}, sumAlt{};
    for (auto n : points) {
        sumLat += n.lat;
        sumLng += n.lng;
        sumAlt += n.alt;
    }

    return { // centroid position
        sumLat / points.size(), 
        sumLng / points.size(),
        sumAlt / points.size()
    };
};


} // namespace terrain