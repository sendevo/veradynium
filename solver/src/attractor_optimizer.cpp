#include "../include/attractor_optimizer.h"

terrain::LatLngAlt AttractorOptimizer::findOptimalGatewayPosition() {
    // Find centroid of unconnected devices
    terrain::LatLngAlt centroid = {0.0, 0.0, 0.0};
    int unconnected_count = 0;
    
    for(const auto& device : network.getEndDevices()) {
        if(device.assigned_gateway == nullptr) {
            centroid += device.location;
            unconnected_count++;
        }
    }
    
    if(unconnected_count > 0) {
        centroid.lat /= unconnected_count;
        centroid.lng /= unconnected_count;
        centroid.alt /= unconnected_count;
        
        // Add some randomness to avoid exact overlap
        static std::uniform_real_distribution<> noise(-0.001, 0.001);
        
        centroid.lat += noise(global::gen);
        centroid.lng += noise(global::gen);
        centroid.alt += noise(global::gen);
        
        return centroid;
    }
    
    return {0.0, 0.0, 0.0}; // Invalid position if no unconnected devices
};

terrain::LatLngAlt AttractorOptimizer::findMaxDensityPosition() {
    std::vector<double> bbox = network.getBoundingBox();
    
    double best_density = 0.0;
    terrain::LatLngAlt best_position = {0.0, 0.0, 0.0};

    double lat_step = (bbox[3] - bbox[1]) / network.getElevationGrid().getNumLatitudes();
    double lng_step = (bbox[2] - bbox[0]) / network.getElevationGrid().getNumLongitudes();
    
    for(size_t i = 0; i < network.getElevationGrid().getNumLatitudes(); ++i) {
        for(size_t j = 0; j < network.getElevationGrid().getNumLongitudes(); ++j) {
            terrain::LatLngAlt test_pos = {
                bbox[1] + i * lat_step,
                bbox[0] + j * lng_step,
                2.0
            };
            
            // Count unconnected devices within range
            int nearby_unconnected = 0;
            for(const auto& device : network.getEndDevices()) {
                if(device.assigned_gateway == nullptr) {
                    double distance = network.getElevationGrid().equirectangularDistance(test_pos, device.location);
                    if(distance < network::MAX_RANGE) { // Define this constant
                        nearby_unconnected++;
                    }
                }
            }
            
            if(nearby_unconnected > best_density) {
                best_density = nearby_unconnected;
                best_position = test_pos;
            }
        }
    }
    
    return best_position;
}

void AttractorOptimizer::optimize(unsigned int maxIterations) {

    std::vector<double> bbox = network.getBoundingBox();

    static std::uniform_real_distribution<> disLat(bbox[1], bbox[3]);
    static std::uniform_real_distribution<> disLng(bbox[0], bbox[2]);
    static std::uniform_real_distribution<> disAlt(2.0, 10.0); // Altitude between 2m and 10m for antennas

    // add first gateway at random position
    terrain::LatLngAlt initial_pos = {disLat(global::gen), disLng(global::gen), disAlt(global::gen)};
    network.addGateway(initial_pos);
    global::dbg << "Initial gateway added at (lat: " << initial_pos.lat 
              << ", lng: " << initial_pos.lng 
              << ", alt: " << initial_pos.alt << ")" << std::endl;

    int stagnant_iterations = 0;
    int gateways_added = 1; // Start with one gateway

    for(unsigned int iter = 0; iter < maxIterations; iter++){

        global::dbg << "Iteration " << iter+1 << "/" << maxIterations << std::endl;

        network.connect(); // This disconnects before connecting

        // Not connected end-devices count
        const std::size_t nced = network.getEndDevices().size() - network.getConnectedEdCount();

        if(nced == 0){
            global::dbg << "All devices connected at iteration " << iter << std::endl;
            break;
        }

        // Used as vector
        std::vector<terrain::LatLngAlt> velocities(network.getGateways().size(), {0.0, 0.0, 0.0});

        #pragma omp parallel for schedule(dynamic) // parallelize over gateways
        for(std::size_t g = 0; g < network.getGateways().size(); g++) {
            terrain::LatLngAlt total_force = {0.0, 0.0, 0.0}; // Thread-local
            
            for(std::size_t e = 0; e < network.getEndDevices().size(); e++) {
                terrain::LatLngAlt vec = network.getEndDeviceLocation(e) - network.getGatewayLocation(g);
                terrain::LatLngAlt acc;
                
                if(network.getEndDevices()[e].assigned_gateway == &network.getGateways()[g]) { // Connected to this gateway
                    acc = vec / network.getEndDevices().size(); 
                } else { // Not connected to this gateway
                    acc = vec / nced; 
                }
                total_force += acc;
            }

            velocities[g] = total_force;
            network.translateGateway(g, velocities[g]);
        }

        double total_velocity = 0.0;
        for (const auto& vel : velocities) {
            total_velocity += std::sqrt(vel.lat*vel.lat + vel.lng*vel.lng);
        }
        double avg_velocity = total_velocity / velocities.size();

        global::dbg << "Average gateway velocity: " << avg_velocity << " meters/iteration" << std::endl;
        
        // Check for stagnation
        if(avg_velocity < STAGNATION_THRESHOLD) {
            stagnant_iterations++;
            
            // If stagnated for enough iterations and still have unconnected devices
            if(stagnant_iterations >= STAGNATION_PATIENCE && nced > 0 && gateways_added < MAX_GATEWAYS_TO_ADD) {
                
                // Strategy 1: Add gateway near unconnected device cluster
                terrain::LatLngAlt new_position = findOptimalGatewayPosition();

                // Strategy 2: If strategy 1 fails, use random position
                if(new_position.lat == 0.0 && new_position.lng == 0.0) {
                    global::dbg << "No unconnected devices found for optimal placement, using random position." << std::endl;
                    new_position = {disLat(global::gen), disLng(global::gen), disAlt(global::gen)};
                }
                
                network.addGateway(new_position);
                gateways_added++;
                stagnant_iterations = 0; // Reset stagnation counter
                
                global::dbg << "Added gateway #" << gateways_added 
                          << " at iteration " << iter 
                          << " (lat: " << new_position.lat 
                          << ", lng: " << new_position.lng 
                          << ", alt: " << new_position.alt << ")" << std::endl;
                
                // Resize velocities vector for new gateway
                velocities.resize(network.getGateways().size());
            }
        } else {
            global::dbg << "System still moving -> next iteration." << std::endl;
            stagnant_iterations = 0; // Reset if movement detected            
        }
        
        // Break if velocity is extremely low and no more gateways to add
        if(avg_velocity < STAGNATION_THRESHOLD && gateways_added >= MAX_GATEWAYS_TO_ADD) {
            global::dbg << "Maximum gateways added and system stagnated at iteration " << iter << std::endl;
            break;
        }
    }

    return;
};