void AttractorOptimizer::optimize(unsigned int maxIterations) {
    std::vector<double> bbox = network.getBoundingBox();

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> disLat(bbox[1], bbox[3]);
    static std::uniform_real_distribution<> disLng(bbox[0], bbox[2]);
    static std::uniform_real_distribution<> disAlt(MIN_ALTITUDE, MAX_ALTITUDE); // Define these constants

    // Stagnation detection parameters
    const double STAGNATION_THRESHOLD = 0.00001;
    const int STAGNATION_PATIENCE = 10; // Iterations to wait before adding gateway
    const int MAX_GATEWAYS_TO_ADD = 5; // Prevent infinite gateway addition
    
    int stagnant_iterations = 0;
    int gateways_added = 0;
    double prev_avg_velocity = std::numeric_limits<double>::max();

    for(unsigned int iter = 0; iter < maxIterations; iter++){
        network.connect(); // This disconnects before connecting

        // Not connected end-devices count
        const std::size_t nced = network.getEndDevices().size() - network.getConnectedEdCount();

        if(nced == 0) {
            std::cout << "All devices connected at iteration " << iter << std::endl;
            break;
        }
        
        std::vector<terrain::LatLngAlt> velocities(network.getGateways().size());

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

        double avg_velocity = calculateVelocityMagnitude(velocities);
        
        // Check for stagnation
        if(avg_velocity < STAGNATION_THRESHOLD) {
            stagnant_iterations++;
            
            // If stagnated for enough iterations and still have unconnected devices
            if(stagnant_iterations >= STAGNATION_PATIENCE && nced > 0 && gateways_added < MAX_GATEWAYS_TO_ADD) {
                
                // Strategy 1: Add gateway near unconnected device cluster
                terrain::LatLngAlt new_position = findOptimalGatewayPosition();
                
                // Strategy 2: If strategy 1 fails, use random position
                if(new_position.latitude == 0.0 && new_position.longitude == 0.0) {
                    new_position = {disLat(gen), disLng(gen), disAlt(gen)};
                }
                
                network.addGateway(new_position);
                gateways_added++;
                stagnant_iterations = 0; // Reset stagnation counter
                
                std::cout << "Added gateway #" << gateways_added 
                          << " at iteration " << iter 
                          << " (lat: " << new_position.latitude 
                          << ", lng: " << new_position.longitude 
                          << ", alt: " << new_position.altitude << ")" << std::endl;
                
                // Resize velocities vector for new gateway
                velocities.resize(network.getGateways().size());
            }
        } else {
            stagnant_iterations = 0; // Reset if movement detected
        }
        
        // Break if velocity is extremely low and no more gateways to add
        if(avg_velocity < STAGNATION_THRESHOLD && gateways_added >= MAX_GATEWAYS_TO_ADD) {
            std::cout << "Maximum gateways added and system stagnated at iteration " << iter << std::endl;
            break;
        }
        
        prev_avg_velocity = avg_velocity;
    }

    std::cout << "Optimization completed. Total gateways added: " << gateways_added << std::endl;
    return;
}

// Helper method to find optimal position for new gateway
terrain::LatLngAlt AttractorOptimizer::findOptimalGatewayPosition() {
    // Find centroid of unconnected devices
    terrain::LatLngAlt centroid = {0.0, 0.0, 0.0};
    int unconnected_count = 0;
    
    for(const auto& device : network.getEndDevices()) {
        if(device.assigned_gateway == nullptr) {
            centroid.latitude += device.location.latitude;
            centroid.longitude += device.location.longitude;
            centroid.altitude += device.location.altitude;
            unconnected_count++;
        }
    }
    
    if(unconnected_count > 0) {
        centroid.latitude /= unconnected_count;
        centroid.longitude /= unconnected_count;
        centroid.altitude /= unconnected_count;
        
        // Add some randomness to avoid exact overlap
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<> noise(-0.001, 0.001);
        
        centroid.latitude += noise(gen);
        centroid.longitude += noise(gen);
        centroid.altitude += noise(gen);
        
        return centroid;
    }
    
    return {0.0, 0.0, 0.0}; // Invalid position if no unconnected devices
}

// Alternative: Add gateway at position with maximum unconnected device density
terrain::LatLngAlt AttractorOptimizer::findMaxDensityPosition() {
    std::vector<double> bbox = network.getBoundingBox();
    const int GRID_SIZE = 20; // Adjust resolution as needed
    
    double best_density = 0.0;
    terrain::LatLngAlt best_position = {0.0, 0.0, 0.0};
    
    double lat_step = (bbox[3] - bbox[1]) / GRID_SIZE;
    double lng_step = (bbox[2] - bbox[0]) / GRID_SIZE;
    
    for(int i = 0; i < GRID_SIZE; ++i) {
        for(int j = 0; j < GRID_SIZE; ++j) {
            terrain::LatLngAlt test_pos = {
                bbox[1] + i * lat_step,
                bbox[0] + j * lng_step,
                (MIN_ALTITUDE + MAX_ALTITUDE) / 2.0
            };
            
            // Count unconnected devices within range
            int nearby_unconnected = 0;
            for(const auto& device : network.getEndDevices()) {
                if(device.assigned_gateway == nullptr) {
                    double distance = calculateDistance(test_pos, device.location);
                    if(distance < MAX_GATEWAY_RANGE) { // Define this constant
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