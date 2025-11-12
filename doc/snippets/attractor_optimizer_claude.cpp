#include "attractor_optimizer_claude.h"

bool AttractorOptimizerClaude::isValidGatewayPosition(const terrain::LatLngAlt& pos) {
    // Check if position is too close to existing gateways
    for(const auto& gateway : network.getGateways()) {
        double dist = network.getElevationGrid().equirectangularDistance(pos, gateway.location);
        if(dist < MIN_GATEWAY_SEPARATION) {
            return false;
        }
    }
    return true;
}

double AttractorOptimizerClaude::calculateCoverage() {
    network.connect();
    return static_cast<double>(network.getConnectedEdCount()) / network.getEndDevices().size();
}

terrain::LatLngAlt AttractorOptimizerClaude::findOptimalGatewayPosition() {
    // Strategy 1: Find largest cluster of unconnected devices
    terrain::LatLngAlt best_position = {0.0, 0.0, 0.0};
    int max_coverage = 0;
    
    // Sample positions around unconnected devices
    for(const auto& device : network.getEndDevices()) {
        if(device.assigned_gateway == nullptr) {
            // Test position at this device's location
            terrain::LatLngAlt test_pos = device.location;
            test_pos.alt = 5.0; // Standard gateway height
            
            if(!isValidGatewayPosition(test_pos)) {
                continue;
            }
            
            // Count how many unconnected devices this would cover
            int coverage = 0;
            for(const auto& other_device : network.getEndDevices()) {
                if(other_device.assigned_gateway == nullptr) {
                    double distance = network.getElevationGrid().equirectangularDistance(
                        test_pos, other_device.location);
                    if(distance < network::MAX_RANGE) {
                        coverage++;
                    }
                }
            }
            
            if(coverage > max_coverage) {
                max_coverage = coverage;
                best_position = test_pos;
            }
        }
    }
    
    // Strategy 2: Use density-based placement if Strategy 1 finds nothing
    if(max_coverage == 0) {
        best_position = findMaxDensityPosition();
    }
    
    // Strategy 3: Add randomness if still invalid
    if(best_position.lat == 0.0 && best_position.lng == 0.0) {
        std::vector<double> bbox = network.getBoundingBox();
        std::uniform_real_distribution<> disLat(bbox[1], bbox[3]);
        std::uniform_real_distribution<> disLng(bbox[0], bbox[2]);
        std::uniform_real_distribution<> disAlt(3.0, 8.0);
        
        int attempts = 0;
        do {
            best_position = {disLat(global::gen), disLng(global::gen), disAlt(global::gen)};
            attempts++;
        } while(!isValidGatewayPosition(best_position) && attempts < 100);
    }
    
    return best_position;
}

terrain::LatLngAlt AttractorOptimizerClaude::findMaxDensityPosition() {
    std::vector<double> bbox = network.getBoundingBox();
    
    double best_density = 0.0;
    terrain::LatLngAlt best_position = {0.0, 0.0, 0.0};

    // Use coarser grid for efficiency
    const int SAMPLE_POINTS = 20;
    double lat_step = (bbox[3] - bbox[1]) / SAMPLE_POINTS;
    double lng_step = (bbox[2] - bbox[0]) / SAMPLE_POINTS;
    
    for(int i = 0; i < SAMPLE_POINTS; ++i) {
        for(int j = 0; j < SAMPLE_POINTS; ++j) {
            terrain::LatLngAlt test_pos = {
                bbox[1] + (i + 0.5) * lat_step,
                bbox[0] + (j + 0.5) * lng_step,
                5.0
            };
            
            if(!isValidGatewayPosition(test_pos)) {
                continue;
            }
            
            // Count unconnected devices within range
            int nearby_unconnected = 0;
            for(const auto& device : network.getEndDevices()) {
                if(device.assigned_gateway == nullptr) {
                    double distance = network.getElevationGrid().equirectangularDistance(
                        test_pos, device.location);
                    if(distance < network::MAX_RANGE) {
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

void AttractorOptimizerClaude::optimize(unsigned int maxIterations) {
    std::vector<double> bbox = network.getBoundingBox();

    std::uniform_real_distribution<> disLat(bbox[1], bbox[3]);
    std::uniform_real_distribution<> disLng(bbox[0], bbox[2]);
    std::uniform_real_distribution<> disAlt(3.0, 8.0);

    // Add first gateway at center of bounding box (better than random)
    terrain::LatLngAlt initial_pos = {
        (bbox[1] + bbox[3]) / 2.0,
        (bbox[0] + bbox[2]) / 2.0,
        5.0
    };
    network.addGateway(initial_pos);
    global::dbg << "Initial gateway added at center: (lat: " << initial_pos.lat 
              << ", lng: " << initial_pos.lng 
              << ", alt: " << initial_pos.alt << ")" << std::endl;

    int stagnant_iterations = 0;
    int gateways_added = 1;
    
    // Persistent velocities for damping
    std::vector<terrain::LatLngAlt> velocities(network.getGateways().size(), {0.0, 0.0, 0.0});
    double prev_coverage = 0.0;

    for(unsigned int iter = 0; iter < maxIterations; iter++) {
        global::dbg << "Iteration " << iter+1 << "/" << maxIterations << std::endl;

        network.connect();

        const std::size_t nced = network.getEndDevices().size() - network.getConnectedEdCount();
        double current_coverage = calculateCoverage();

        if(nced == 0) {
            global::dbg << "All devices connected at iteration " << iter << std::endl;
            break;
        }

        global::dbg << "Coverage: " << (current_coverage * 100.0) << "% (" 
                   << network.getConnectedEdCount() << "/" 
                   << network.getEndDevices().size() << " devices)" << std::endl;

        // Resize velocities if gateways were added
        velocities.resize(network.getGateways().size(), {0.0, 0.0, 0.0});

        #pragma omp parallel for schedule(dynamic)
        for(std::size_t g = 0; g < network.getGateways().size(); g++) {
            terrain::LatLngAlt attractive_force = {0.0, 0.0, 0.0};
            terrain::LatLngAlt repulsive_force = {0.0, 0.0, 0.0};
            
            int connected_to_this = 0;
            int unconnected_nearby = 0;
            
            // Attraction from devices
            for(std::size_t e = 0; e < network.getEndDevices().size(); e++) {
                const auto& device = network.getEndDevices()[e];
                terrain::LatLngAlt vec = device.location - network.getGatewayLocation(g);
                double distance = network.getElevationGrid().equirectangularDistance(
                    device.location, network.getGatewayLocation(g));
                
                if(distance < 1e-6) continue; // Avoid division by zero
                
                // Normalize vector
                double mag = std::sqrt(vec.lat*vec.lat + vec.lng*vec.lng + vec.alt*vec.alt);
                if(mag < 1e-6) continue;
                
                terrain::LatLngAlt normalized = {vec.lat/mag, vec.lng/mag, vec.alt/mag};
                
                if(device.assigned_gateway == &network.getGateways()[g]) {
                    // Weaker attraction to connected devices (keep them close)
                    attractive_force += normalized * (1.0 / (1.0 + distance/1000.0));
                    connected_to_this++;
                } else if(device.assigned_gateway == nullptr) {
                    // Stronger attraction to unconnected devices
                    double strength = 3.0 / (1.0 + distance/500.0);
                    attractive_force += normalized * strength;
                    unconnected_nearby++;
                }
            }
            
            // Repulsion from other gateways (prevent clustering)
            for(std::size_t other_g = 0; other_g < network.getGateways().size(); other_g++) {
                if(other_g == g) continue;
                
                terrain::LatLngAlt vec = network.getGatewayLocation(g) - network.getGatewayLocation(other_g);
                double distance = network.getElevationGrid().equirectangularDistance(
                    network.getGatewayLocation(g), network.getGatewayLocation(other_g));
                
                if(distance < 1e-6) {
                    // Add random displacement if gateways overlap
                    vec = {disLat(global::gen) * 0.001, disLng(global::gen) * 0.001, 0.0};
                    distance = 1.0;
                }
                
                double mag = std::sqrt(vec.lat*vec.lat + vec.lng*vec.lng + vec.alt*vec.alt);
                if(mag < 1e-6) continue;
                
                terrain::LatLngAlt normalized = {vec.lat/mag, vec.lng/mag, vec.alt/mag};
                
                // Strong repulsion at close range
                double repulsion_strength = GATEWAY_REPULSION_STRENGTH * (MIN_GATEWAY_SEPARATION / (distance + 1.0));
                repulsive_force += normalized * repulsion_strength;
            }
            
            // Combine forces with damping
            terrain::LatLngAlt total_force = attractive_force + repulsive_force;
            velocities[g] = velocities[g] * DAMPING_FACTOR + total_force * LEARNING_RATE;
            
            // Translate gateway
            network.translateGateway(g, velocities[g]);
        }

        // Calculate average velocity
        double total_velocity = 0.0;
        for(const auto& vel : velocities) {
            total_velocity += std::sqrt(vel.lat*vel.lat + vel.lng*vel.lng);
        }
        double avg_velocity = total_velocity / velocities.size();

        global::dbg << "Average gateway velocity: " << avg_velocity << std::endl;
        
        // Check for stagnation - more aggressive gateway addition
        bool coverage_improved = (current_coverage - prev_coverage) > 0.01; // More lenient threshold
        bool should_add_gateway = false;
        
        if(avg_velocity < STAGNATION_THRESHOLD) {
            stagnant_iterations++;
        } else {
            stagnant_iterations = 0;
        }
        
        // Multiple triggers for adding gateways
        if(nced > 0 && gateways_added < MAX_GATEWAYS_TO_ADD) {
            // Trigger 1: Stagnation
            if(stagnant_iterations >= STAGNATION_PATIENCE) {
                should_add_gateway = true;
                global::dbg << "Trigger: Stagnation detected" << std::endl;
            }
            // Trigger 2: High percentage of unconnected devices
            else if(current_coverage < 0.8 && iter > 20) {
                should_add_gateway = true;
                global::dbg << "Trigger: Low coverage (<80%)" << std::endl;
            }
            // Trigger 3: No improvement in last several iterations
            else if(!coverage_improved && iter > 10 && (iter % 10 == 0)) {
                should_add_gateway = true;
                global::dbg << "Trigger: No coverage improvement" << std::endl;
            }
        }
        
        if(should_add_gateway) {
            terrain::LatLngAlt new_position = findOptimalGatewayPosition();

            if(new_position.lat != 0.0 || new_position.lng != 0.0) {
                network.addGateway(new_position);
                gateways_added++;
                stagnant_iterations = 0;
                
                global::dbg << "Added gateway #" << gateways_added 
                          << " at iteration " << iter 
                          << " (lat: " << new_position.lat 
                          << ", lng: " << new_position.lng 
                          << ", alt: " << new_position.alt << ")" << std::endl;
                
                velocities.resize(network.getGateways().size(), {0.0, 0.0, 0.0});
            }
        }
        
        prev_coverage = current_coverage;
        
        // Early termination if no improvement possible
        if(gateways_added >= MAX_GATEWAYS_TO_ADD && stagnant_iterations >= STAGNATION_PATIENCE) {
            global::dbg << "Maximum gateways added and no improvement at iteration " << iter << std::endl;
            break;
        }
    }

    // Final connection and report
    network.connect();
    double final_coverage = calculateCoverage();
    global::dbg << "\n=== Optimization Complete ===" << std::endl;
    global::dbg << "Total gateways: " << network.getGateways().size() << std::endl;
    global::dbg << "Final coverage: " << (final_coverage * 100.0) << "%" << std::endl;
    global::dbg << "Connected devices: " << network.getConnectedEdCount() 
               << "/" << network.getEndDevices().size() << std::endl;
}