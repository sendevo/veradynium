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

    void optimize(int maxIterations = 500, double minSpeed = 0.01, double acceleration = 0.02);

private:
    terrain::LatLngAlt computeCentroid(const network::Gateway& gw, double fallbackLat = 0.0, double fallbackLng = 0.0);    
    
    network::Network& network;
    int iterations;        // Number of iterations taken to converge
};

} // namespace kmean