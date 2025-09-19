#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "json.hpp"
#include "feature_collection.hpp"
#include "detail.hpp"
#include "terrain.hpp"

/**
 * 
 * @brief Models a LoRaWAN network with Gateways and End Devices, with terrain elevation awareness.
 * 
 */
namespace network {

constexpr double MAX_DISTANCE = 2000; // Maximum distance (in meters) for a valid connection = 2km

class Node {
public:
    Node() = default;
    Node(const std::string& id, terrain::LatLngAlt pos) : id(id), location(pos) {}
    
    std::string id;
    
    terrain::LatLngAlt location;
    
    static inline Node parse(const nlohmann::json& properties, double lat, double lng) {
        std::string id = detail::require_string(properties, "id");
        double height = detail::optional_number(properties, "height", 0.0);
        return Node{id, {lat, lng, height}};
    };

    inline double distanceTo(const Node& other, const terrain::ElevationGrid& grid) const {
        return grid.distance(location, other.location);
    }
    
    inline bool lineOfSightTo(const Node& other, const terrain::ElevationGrid& grid) const {
        return grid.lineOfSight(location, other.location);
    }
};


// Forward declaration to avodd circular dependency
// (Gateway has vector of EndDevice pointers and EndDevice has pointer to Gateway)
class Gateway;   
class EndDevice;


class EndDevice : public Node {
public:
    EndDevice() = default;
    EndDevice(const std::string& id, terrain::LatLngAlt pos) : Node(id, pos) {}
    Gateway* assigned_gateway = nullptr; // Pointer to assigned gateway
    double distance_to_gateway = std::numeric_limits<double>::max(); // Distance to assigned gateway
};


class Gateway : public Node {
public:
    Gateway() = default;
    Gateway(const std::string& id, terrain::LatLngAlt pos) : Node(id, pos) {}
    std::vector<EndDevice*> connected_devices; // Pointers to connected end devices
};

class Network {
public:
    Network() = default;

    Network(const std::vector<Gateway>& gws,
            const std::vector<EndDevice>& eds,
            const terrain::ElevationGrid& grid)
        : gateways(gws), end_devices(eds), elevation_grid(grid) {}
    
    inline void setElevationGrid(const terrain::ElevationGrid& grid) {elevation_grid = grid;};
    
    static Network fromGeoJSON(const std::string& filepath);
    static Network fromFeatureCollection(const geojson::FeatureCollection& fc);
    geojson::FeatureCollection toFeatureCollection() const;
    inline std::vector<double> getBoundingBox() const { return bbox; };
    
    unsigned int connect();
    void disconnect();

    std::vector<Gateway> gateways;
    std::vector<EndDevice> end_devices;
    terrain::ElevationGrid elevation_grid;

    void print(global::PRINT_TYPE format = global::PLAIN_TEXT);

private:
    double total_distance;
    std::vector<double> bbox; // Bbox of features
    void printPlainText() const;
    void printJSON() const;
};

} // namespace network