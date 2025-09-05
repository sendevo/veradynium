#include "../include/network.hpp"

namespace network {

Network Network::fromJSON(const std::string& filepath, terrain::ElevationGrid grid) {
    
    Network network;
    network.elevation_grid = grid;

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

    network.computeDistanceMatrix();

    return network;
}

void Network::printInfo() const {
    std::cout << "Network Information:" << std::endl;
    std::cout << "Number of Gateways: " << gateways.size() << std::endl;
    for (const auto& gw : gateways) {
        std::cout << "  Gateway ID: " << gw.id 
                  << ", Lat: " << gw.lat 
                  << ", Lng: " << gw.lng 
                  << ", Height: " << gw.height << "m" << std::endl;
    }
    std::cout << "Number of End Devices: " << end_devices.size() << std::endl;
    for (const auto& ed : end_devices) {
        std::cout << "  End Device ID: " << ed.id 
                  << ", Lat: " << ed.lat 
                  << ", Lng: " << ed.lng 
                  << ", Height: " << ed.height << "m" << std::endl;
    }
}

void Network::printDistanceMatrix() const {
    std::cout << "Distance Matrix (Gateways to End Devices):" << std::endl;
    std::cout << "      ";
    for (const auto& ed : end_devices) {
        std::cout << ed.id << "     ";
    }
    std::cout << std::endl;

    for (size_t i = 0; i < gateways.size(); ++i) {
        std::cout << gateways[i].id << "  ";
        for (size_t j = 0; j < end_devices.size(); ++j) {
            if (distance_matrix[i][j] < 0) {
                std::cout << " No LOS ";
            } else {
                std::cout << std::fixed << std::setprecision(1) << distance_matrix[i][j] << "m ";
            }
        }
        std::cout << std::endl;
    }
}

void Network::computeDistanceMatrix() {
    size_t num_gws = gateways.size();
    size_t num_eds = end_devices.size();
    
    distance_matrix = std::vector<std::vector<double>>(num_gws, std::vector<double>(num_eds, false));

    for(size_t i = 0; i < num_gws; i++) {
        
        const auto& gw = gateways[i];
        
        for(size_t j = 0; j < num_eds; j++) {
            
            const auto& ed = end_devices[j];
            
            bool los = elevation_grid.lineOfSight(gw.lat, gw.lng, ed.lat, ed.lng, gw.height, ed.height);
            double dist = elevation_grid.distance(gw.lat, gw.lng, ed.lat, ed.lng, gw.height, ed.height);
            
            if(los)
                distance_matrix[i][j] = dist;
            else
                distance_matrix[i][j] = -1.0; // Indicate no LOS with -1
        }
    }
}

} // namespace network