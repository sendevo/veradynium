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
            network.gateways.push_back(Gateway::parse_gateway(properties, lat, lng));
        } else if(type == "end_device") {
            network.end_devices.push_back(EndDevice::parse_end_device(properties, lat, lng));
        } else {
            throw std::runtime_error("Invalid GeoJSON: unknown feature type '" + type + "'");
        }
    }

    return network;
};

/* NOT PARALELIZED VERSION
void Network::assignDevices() { // For each end-device, assigns its pointer to the closest reachable gateway.
    // First, clear previous connections
    for (auto& gw : gateways) {
        gw.connected_devices.clear();
    }
    for (auto& dev : end_devices) {
        dev.assigned_gateway = nullptr;
    }

    // Assign each end device to the closest reachable gateway
    for (auto& dev : end_devices) {
        double minDist = std::numeric_limits<double>::max(); // Start with a large distance
        network::Gateway* closestGateway = nullptr;

        for (auto& gw : gateways) {
            if (elevation_grid.lineOfSight(gw.lat, gw.lng, dev.lat, dev.lng, gw.height, dev.height)) {
                double dist = elevation_grid.distance(gw.lat, gw.lng, dev.lat, dev.lng, gw.height, dev.height);
                if (dist < minDist) {
                    minDist = dist;
                    closestGateway = &gw;
                }
            }
        }

        // Assign device to closest gateway if reachable
        if (closestGateway) {
            dev.assigned_gateway = closestGateway;
            closestGateway->connected_devices.push_back(&dev);
        } 
        // else device remains unassigned
    }
}
*/

void Network::assignDevices() {
    const size_t num_gws = gateways.size();
    const size_t num_eds = end_devices.size();

    // Phase 1: parallel per-device search for best gateway
    std::vector<int> best_gw_idx(num_eds, -1);
    std::vector<double> best_dist(num_eds, std::numeric_limits<double>::infinity());

    #pragma omp parallel for schedule(dynamic) // parallelize over end devices
    for (int j = 0; j < static_cast<int>(num_eds); ++j) {
        double minDist = std::numeric_limits<double>::infinity();
        int best = -1;

        for (int i = 0; i < static_cast<int>(num_gws); ++i) {
            const auto& gw = gateways[i];
            const auto& ed = end_devices[j];

            // call combined LOS+distance (fast single call)
            bool los =     elevation_grid.lineOfSight(gw.lat, gw.lng, ed.lat, ed.lng, gw.height, ed.height);
            double distance = elevation_grid.distance(gw.lat, gw.lng, ed.lat, ed.lng, gw.height, ed.height);

            if (los && distance < minDist) {
                minDist = distance;
                best = i;
            }
        }

        best_gw_idx[j] = best;
        best_dist[j] = minDist;
    }

    // Phase 2: clear and populate connections (sequential)
    for (auto& gw : gateways) gw.connected_devices.clear();
    for (auto& dev : end_devices) dev.assigned_gateway = nullptr;

    for (size_t j = 0; j < num_eds; ++j) {
        int best = best_gw_idx[j];
        if (best >= 0) {
            end_devices[j].assigned_gateway = &gateways[best];
            gateways[best].connected_devices.push_back(&end_devices[j]);
        }
    }
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
};

} // namespace network