#include "../include/network.hpp"

namespace network {

Network Network::fromJSON(const std::string& filepath) {
    Network network;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open JSON file: " + filepath);
    }

    nlohmann::json j;
    file >> j;

    if(j.value("type", "") != "FeatureCollection"){
        throw std::runtime_error("Invalid GeoJSON: expected 'FeatureCollection'");
    }

    for (const auto& feature : j["features"]) {
        if(feature.value("type", "") != "Feature") {
            throw std::runtime_error("Invalid GeoJSON: feature missing or incorrect 'type'");
        }

        const auto& properties = feature.at("properties");
        const auto& geometry   = feature.at("geometry");

        if(geometry.value("type", "") != "Point") {
            throw std::runtime_error("Invalid GeoJSON: only 'Point' geometries are supported.");
        }

        auto coords = geometry.at("coordinates");
        if(!coords.is_array() || coords.size() < 2) {
            throw std::runtime_error("Invalid GeoJSON: invalid coordinates.");
        }

        double lng = coords[0];
        double lat = coords[1];

        std::string type = detail::require_string(properties, "type");

        if(type == "gateway") {
            network.gateways.push_back(parse_gateway(properties, lat, lng));
        } else if(type == "end_device") {
            network.end_devices.push_back(parse_end_device(properties, lat, lng));
        } else {
            throw std::runtime_error("Invalid GeoJSON: unknown feature type '" + type + "'");
        }
    }

    return network;
}

} // namespace network