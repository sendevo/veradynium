#include "../include/kmean.hpp"

namespace kmean {

void KMeansOptimizer::optimize(int maxIterations, double epsilon) {
    
    iterations = 0;

    // Remove all gateways from network
    for (auto& gw : network.gateways) {
        gw.connected_devices.clear();
    }
    for (auto& ed : network.end_devices) {
        ed.assigned_gateway = nullptr;
    }

    network.gateways.clear();

    for (int i = 0; i < maxIterations; ++i) {
        // Step 1: Assignment
        network.assignDevices(); // First pass will do nothing

        // Check for empty gateways and re-assign them to a random unconnected device if necessary
        for (auto& gw : network.gateways) {
            if (gw.connected_devices.empty()) {
                // Find an unassigned device to move this gateway to
                network::EndDevice* unassigned_device = nullptr;
                for (auto& ed : network.end_devices) {
                    if (ed.assigned_gateway == nullptr) {
                        unassigned_device = &ed;
                        break;
                    }
                }
                if (unassigned_device) {
                    gw.lat = unassigned_device->lat;
                    gw.lng = unassigned_device->lng;
                }
            }
        }

        double max_movement = 0.0;
        std::vector<terrain::LatLngAlt> old_positions;
        for (const auto& gw : network.gateways) {
            old_positions.push_back({gw.lat, gw.lng, gw.height});
        }
        
        // Step 2: Update
        for (size_t j = 0; j < network.gateways.size(); ++j) {
            auto new_pos = computeCentroid(network.gateways[j]);
            network.gateways[j].lat = new_pos.lat;
            network.gateways[j].lng = new_pos.lng;

            double movement = network.elevation_grid.distance(
                old_positions[j].lat, old_positions[j].lng, new_pos.lat, new_pos.lng, 0, 0);
            if (movement > max_movement) {
                max_movement = movement;
            }
        }
        
        iterations = i + 1;

        // Step 3: Convergence check
        if (max_movement < epsilon) {
            break;
        }
    }
};

terrain::LatLngAlt KMeansOptimizer::computeCentroid(const network::Gateway& gw) {
    // Computes the centroid of the connected devices to the gateway
    if (gw.connected_devices.empty()) {
        return {gw.lat, gw.lng, gw.height}; // No connected devices, return gateway position
    }

    double sumLat = 0.0;
    double sumLng = 0.0;
    for (auto d : gw.connected_devices) {
        sumLat += d->lat;
        sumLng += d->lng;
    }

    return { // centroid position
        sumLat / gw.connected_devices.size(), 
        sumLng / gw.connected_devices.size(), 
        gw.height
    };
}

} // namespace kmean