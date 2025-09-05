#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "json.hpp"
#include "detail.hpp"
#include "terrain.hpp"

/**
 * 
 * @brief Network elements: gateways and end devices
 * 
 */
namespace network {

class Node {
public:
    Node() = default;
    Node(const std::string& id, double lat, double lng, double height = 0.0)
        : id(id), lat(lat), lng(lng), height(height) {}
    std::string id;
    double lat;
    double lng;
    double height; // Height above ground in meters
    inline double distanceTo(const Node& other, const terrain::ElevationGrid& grid) const {
        return grid.distance(lat, lng, other.lat, other.lng, height, other.height);
    }
    inline bool lineOfSightTo(const Node& other, const terrain::ElevationGrid& grid) const {
        return grid.lineOfSight(lat, lng, other.lat, other.lng, height, other.height);
    }
};

class EndDevice : public Node {
public:
    EndDevice() = default;
    EndDevice(const std::string& id, double lat, double lng, double height = 0.0)
        : Node(id, lat, lng, height) {}
};

class Gateway : public Node {
public:
    Gateway() = default;
    Gateway(const std::string& id, double lat, double lng, double height)
        : Node(id, lat, lng, height) {}
};

class Network {
public:
    Network() = default;

    Network(const std::vector<Gateway>& gws,
            const std::vector<EndDevice>& eds,
            const terrain::ElevationGrid& grid)
        : gateways(gws), end_devices(eds), elevation_grid(grid) {
            computeDistanceMatrix();
        }
    
    // Build network from GeoJSON file
    static Network fromJSON(const std::string& filepath, terrain::ElevationGrid grid);
    
    void printInfo() const;
    void printDistanceMatrix() const;

private:    
    void computeDistanceMatrix();

    std::vector<Gateway> gateways;
    std::vector<EndDevice> end_devices;
    terrain::ElevationGrid elevation_grid;
    std::vector<std::vector<double>> distance_matrix;
};

static inline Gateway parse_gateway(const nlohmann::json& properties, double lat, double lng) {
    std::string id = detail::require_string(properties, "id");
    double height = detail::require_number(properties, "height");
    return Gateway{id, lat, lng, height};
}

static inline EndDevice parse_end_device(const nlohmann::json& properties, double lat, double lng) {
    std::string id = detail::require_string(properties, "id");
    double height = detail::optional_number(properties, "height", 0.0);
    return EndDevice{id, lat, lng, height};
}

} // namespace network