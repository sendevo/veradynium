#ifndef ATTRACTOR_OPTIMIZER_CLAUDE
#define ATTRACTOR_OPTIMIZER_CLAUDE

#include <vector>
#include <cmath>
#include <limits>
#include "global.hpp"
#include "optimizer.hpp"
#include "network.hpp"

/**
 * @brief Improved Attractor optimization algorithm for Gateway Placement Problem
 */

#define STAGNATION_THRESHOLD 0.01    // Increased - triggers gateway addition sooner
#define STAGNATION_PATIENCE 5        // Reduced - adds gateways more aggressively
#define MAX_GATEWAYS_TO_ADD 50       // Increased - allows more gateways if needed
#define LEARNING_RATE 0.3            // Increased - faster movement
#define DAMPING_FACTOR 0.75          // Reduced - more responsive to forces
#define MIN_GATEWAY_SEPARATION 100.0 // Minimum distance between gateways (meters)
#define GATEWAY_REPULSION_STRENGTH 0.1  // Reduced - less aggressive repulsion

class AttractorOptimizerClaude : public optimizer::Optimizer {
public:
    AttractorOptimizerClaude(network::Network& net) : optimizer::Optimizer(net) {};

    void optimize(unsigned int maxIterations = 500);
    void optimize() override { optimize(500); };
private:
    terrain::LatLngAlt findOptimalGatewayPosition();
    terrain::LatLngAlt findMaxDensityPosition();
    bool isValidGatewayPosition(const terrain::LatLngAlt& pos);
    double calculateCoverage();
};

#endif // ATTRACTOR_OPTIMIZER_CLAUDE