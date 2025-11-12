#ifndef ATTRACTOR_OPTIMIZER_H
#define ATTRACTOR_OPTIMIZER_H

#include <vector>
#include <cmath>
#include "global.hpp"
#include "optimizer.hpp"
#include "network.hpp"

/**
 * 
 * @brief Atracctor optimization algorithm for the Gateway Placement Problem.
 * 
 */

#define STAGNATION_THRESHOLD 0.01
#define STAGNATION_PATIENCE 10 // Iterations to wait before adding gateway
#define MAX_GATEWAYS_TO_ADD 10 // Prevent infinite gateway addition

class AttractorOptimizer : public optimizer::Optimizer {
public:
    AttractorOptimizer(network::Network& net) : optimizer::Optimizer(net) {};

    void optimize(unsigned int maxIterations = 500);
    void optimize() override { optimize(500); };
private:
    terrain::LatLngAlt findOptimalGatewayPosition();
    terrain::LatLngAlt findMaxDensityPosition();
};

#endif // ATTRACTOR_OPTIMIZER_H