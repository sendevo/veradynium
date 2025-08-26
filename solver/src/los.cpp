#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

// ------------------- Structs -------------------
struct Point {
    double lat, lng, alt;
};

// ------------------- CSV Reader -------------------
std::vector<Point> readCSV(const std::string &filename) {
    std::vector<Point> points;
    std::ifstream file(filename);
    if(!file.is_open()) {
        std::cerr << "Cannot open file " << filename << std::endl;
        return points;
    }

    std::string line;
    std::getline(file, line); // skip header

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        Point p;
        std::getline(ss, token, ','); p.lat = std::stod(token);
        std::getline(ss, token, ','); p.lng = std::stod(token);
        std::getline(ss, token, ','); p.alt = std::stod(token);
        points.push_back(p);
    }
    return points;
}

// ------------------- Utility Functions -------------------
int findIndex(const std::vector<double>& arr, double val) {
    auto it = std::lower_bound(arr.begin(), arr.end(), val);
    if(it == arr.end()) return arr.size()-2;
    int idx = std::max(int(it - arr.begin() - 1), 0);
    return idx;
}

// ------------------- Bilinear Interpolation -------------------
double bilinearInterpolation(double lat, double lng,
                             const std::vector<double>& lats,
                             const std::vector<double>& lngs,
                             const std::vector<std::vector<double>>& grid) {
    int i = findIndex(lats, lat);
    int j = findIndex(lngs, lng);

    double x1 = lngs[j], x2 = lngs[j+1];
    double y1 = lats[i], y2 = lats[i+1];
    double Q11 = grid[i][j];
    double Q21 = grid[i][j+1];
    double Q12 = grid[i+1][j];
    double Q22 = grid[i+1][j+1];

    double fxy1 = ((x2-lng)/(x2-x1))*Q11 + ((lng-x1)/(x2-x1))*Q21;
    double fxy2 = ((x2-lng)/(x2-x1))*Q12 + ((lng-x1)/(x2-x1))*Q22;
    double fxy  = ((y2-lat)/(y2-y1))*fxy1 + ((lat-y1)/(y2-y1))*fxy2;
    return fxy;
}

// ------------------- Build Regular Grid -------------------
void buildGrid(const std::vector<Point>& points,
               std::vector<double>& unique_lats,
               std::vector<double>& unique_lngs,
               std::vector<std::vector<double>>& grid) {

    // Extract unique sorted coordinates
    for(const auto& p: points) { unique_lats.push_back(p.lat); unique_lngs.push_back(p.lng); }
    std::sort(unique_lats.begin(), unique_lats.end());
    std::sort(unique_lngs.begin(), unique_lngs.end());
    unique_lats.erase(std::unique(unique_lats.begin(), unique_lats.end()), unique_lats.end());
    unique_lngs.erase(std::unique(unique_lngs.begin(), unique_lngs.end()), unique_lngs.end());

    // Initialize grid
    grid.assign(unique_lats.size(), std::vector<double>(unique_lngs.size(), 0));

    // Fill grid
    for(const auto& p: points) {
        int i = std::distance(unique_lats.begin(), std::lower_bound(unique_lats.begin(), unique_lats.end(), p.lat));
        int j = std::distance(unique_lngs.begin(), std::lower_bound(unique_lngs.begin(), unique_lngs.end(), p.lng));
        if(i >= int(unique_lats.size())) i = unique_lats.size()-1;
        if(j >= int(unique_lngs.size())) j = unique_lngs.size()-1;
        grid[i][j] = p.alt;
    }
}

// ------------------- Line-of-Sight -------------------
bool lineOfSight(double lat1, double lon1, double lat2, double lon2,
                 double h1, double h2,
                 const std::vector<double>& lats,
                 const std::vector<double>& lngs,
                 const std::vector<std::vector<double>>& grid,
                 int num_samples=500) {

    double elev1 = bilinearInterpolation(lat1, lon1, lats, lngs, grid) + h1;
    double elev2 = bilinearInterpolation(lat2, lon2, lats, lngs, grid) + h2;

    for(int k=0; k<=num_samples; ++k){
        double t = k/double(num_samples);
        double lat = lat1 + t*(lat2-lat1);
        double lon = lon1 + t*(lon2-lon1);
        double terrain = bilinearInterpolation(lat, lon, lats, lngs, grid);
        double los = elev1 + t*(elev2-elev1);
        if(terrain > los) return false; // blocked
    }
    return true; // clear
}