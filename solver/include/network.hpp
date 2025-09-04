#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "json.hpp"

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
    static Network fromJSON(const std::string& filepath);

    std::vector<Gateway> gateways;
    std::vector<EndDevice> end_devices;
};

} // namespace network