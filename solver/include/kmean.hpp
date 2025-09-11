#pragma once

#include "network.hpp"
#include "terrain.hpp"

/**
 * 
 * @brief K-Means clustering optimizer for LoRaWAN networks.
 * 
 */

namespace kmean {
    
struct Result {
    double total_distance; // Total distance of all end devices to their assigned gateway
    int iterations;        // Number of iterations taken to converge
};

class KMeansOptimizer {
public:
    KMeansOptimizer(network::Network& net) : network(net) {}

    Result optimize(int maxIterations = 50, double epsilon = 1e-6);

private:
    network::Network& network;
    
    terrain::LatLngAlt computeCentroid(const network::Gateway& gw);
};

} // namespace kmean