#include "../include/kmean.hpp"

namespace kmean {

void KMeansOptimizer::optimize(int maxIterations, double epsilon) {
    
    iterations = 0;

    // Reset connections
    for (auto& gw : network.gateways) {
        gw.connected_devices.clear();
    }
    for (auto& ed : network.end_devices) {
        ed.assigned_gateway = nullptr;
    }

    // Remove all gateways to allocate from scratch
    network.gateways.clear();

    while (true) {
        // Step 1: Check coverage
        network.assignDevices();
        
        bool all_devices_covered = true;
        network::EndDevice* furthest_device = nullptr;
        double max_dist_sq = 0.0;
        
        for (auto& ed : network.end_devices) {
            if (!ed.assigned_gateway) {
                all_devices_covered = false;
                
                // Find the device furthest from any existing gateway
                double min_dist_sq_for_device = std::numeric_limits<double>::infinity();
                for (const auto& gw : network.gateways) {
                    double dist_sq = network.elevation_grid.distance(
                        ed.lat, ed.lng, gw.lat, gw.lng, ed.height, gw.height);
                    if (dist_sq < min_dist_sq_for_device) {
                        min_dist_sq_for_device = dist_sq;
                    }
                }
                
                if (min_dist_sq_for_device > max_dist_sq) {
                    max_dist_sq = min_dist_sq_for_device;
                    furthest_device = &ed;
                }
            }
        }
        
        if (all_devices_covered) {
            break;
        }

        // Step 2: Add a new gateway at the location of the furthest uncovered device
        if (furthest_device) {
            network::Gateway new_gw(
                //global::generate_uuid(), 
                std::to_string(network.gateways.size() + 1), // Simple ID
                furthest_device->lat, 
                furthest_device->lng, 
                2.0 // Default height
            );
           network.gateways.push_back(new_gw);
        } else {
            // No unassigned devices, but all_devices_covered was false.
            // This case should ideally not be reached if logic is correct,
            // but is a safe exit.
            break; 
        }

        // Step 3: Run K-Means-like local optimization on the new set of gateways
        for (int i = 0; i < maxIterations; ++i) {
            network.assignDevices();
            double max_movement = 0.0;
            std::vector<terrain::LatLngAlt> old_positions;
            for (const auto& gw : network.gateways) {
                old_positions.push_back({gw.lat, gw.lng, gw.height});
            }
            
            for (size_t j = 0; j < network.gateways.size(); ++j) {
                if (!network.gateways[j].connected_devices.empty()) {
                    auto new_pos = computeCentroid(network.gateways[j]);
                    network.gateways[j].lat = new_pos.lat;
                    network.gateways[j].lng = new_pos.lng;

                    double movement = network.elevation_grid.distance(
                        old_positions[j].lat, old_positions[j].lng, new_pos.lat, new_pos.lng, 0, 0);
                    if (movement > max_movement) {
                        max_movement = movement;
                    }
                }
            }
            
            if (max_movement < epsilon) {
                break;
            }
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