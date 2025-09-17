#pragma once

#include <vector>
#include <cmath>
#include "network.hpp"

/**
 * 
 * @brief K-Means clustering optimizer for LoRaWAN networks.
 * 
 */

namespace kmean {
    

class KMeansOptimizer {
public:
    KMeansOptimizer(network::Network& net) : network(net) {}

    void optimize(int maxIterations = 50, double epsilon = 1e-6);

private:
    terrain::LatLngAlt computeCentroid(const network::Gateway& gw);    
    
    network::Network& network;
    int iterations;        // Number of iterations taken to converge
};

} // namespace kmean