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


// Forward declaration to avodd circular dependency
// (Gateway has vector of EndDevice pointers and EndDevice has pointer to Gateway)
class Gateway;   
class EndDevice;


class EndDevice : public Node {
public:
    EndDevice() = default;
    EndDevice(const std::string& id, double lat, double lng, double height = 0.0)
        : Node(id, lat, lng, height) {}
    
    static inline EndDevice parse_end_device(const nlohmann::json& properties, double lat, double lng) {
        std::string id = detail::require_string(properties, "id");
        double height = detail::optional_number(properties, "height", 0.0);
        return EndDevice{id, lat, lng, height};
    };

    Gateway* assigned_gateway = nullptr; // Pointer to assigned gateway
};


class Gateway : public Node {
public:
    Gateway() = default;
    Gateway(const std::string& id, double lat, double lng, double height)
        : Node(id, lat, lng, height) {}

    static inline Gateway parse_gateway(const nlohmann::json& properties, double lat, double lng) {
        std::string id = detail::require_string(properties, "id");
        double height = detail::require_number(properties, "height");
        return Gateway{id, lat, lng, height};
    };

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
    
    void print(global::PRINT_TYPE format = global::PLAIN_TEXT);

private:
    void assignDevices();

    void printPlainText() const;
    void printJSON() const;

    std::vector<Gateway> gateways;
    std::vector<EndDevice> end_devices;
    terrain::ElevationGrid elevation_grid;
};

} // namespace network