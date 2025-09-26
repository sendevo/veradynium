#pragma once
#ifndef NETWORK_HPP
#define NETWORK_HPP

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
    
    void connect();
    void disconnect();
    inline const std::size_t getConnectedEdCount() const { return connected_eds_cnt; };

    void print(global::PRINT_TYPE format = global::PLAIN_TEXT);

    inline std::vector<Gateway>& getGateways() { return gateways; };
    inline const std::vector<EndDevice>& getEndDevices() const { return end_devices; };
    inline void addGatewayFront(const Gateway gateway){ gateways.push_back(gateway); }
    inline void addEndDeviceBack(const EndDevice ed){ end_devices.push_back(ed); }
    
    inline const terrain::ElevationGrid& getElevationGrid() const { return elevation_grid; };

    // The following functions do not check bounds
    inline const terrain::LatLngAlt getEndDeviceLocation(size_t index) const { return end_devices[index].location; }
    inline const terrain::LatLngAlt getGatewayLocation(size_t index) const { return gateways[index].location; }
    inline void setEndDeviceLocation(size_t index, terrain::LatLngAlt pos) { end_devices[index].location = pos; }
    inline void setGatewayLocation(size_t index, terrain::LatLngAlt pos) { gateways[index].location = pos; }
    inline void translateEndDevice(size_t index, terrain::LatLngAlt delta) { end_devices[index].location += delta; }
    inline void translateGateway(size_t index, terrain::LatLngAlt delta) { gateways[index].location += delta; }

private:
    std::vector<Gateway> gateways;
    std::vector<EndDevice> end_devices;
    terrain::ElevationGrid elevation_grid;

    double total_distance;
    std::size_t connected_eds_cnt;
    
    std::vector<double> bbox; // Bbox of network
    
    void printPlainText() const;
    void printJSON() const;
};

} // namespace network

#endif // NETWORK_HPP