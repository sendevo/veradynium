#pragma once

#include <vector>
#include <cmath>
#include "network.hpp"

/**
 * 
 * @brief Optimization algorithms for the Gateway Placement Problem.
 * 
 */

namespace optimizers {

class ClusteringOptimizer {
public:
    ClusteringOptimizer(network::Network& net) : network(net) {}

    void optimize(int maxIterations = 500, double minSpeed = 1e-5, double acceleration = 0.05);

private:
    network::Network& network;
};

} // namespace optimizers