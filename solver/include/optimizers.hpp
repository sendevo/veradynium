#pragma once
#ifndef OPTIMIZERS_HPP
#define OPTIMIZERS_HPP

#include <vector>
#include <cmath>
#include "network.hpp"

/**
 * 
 * @brief Optimization algorithms for the Gateway Placement Problem.
 * 
 */

namespace optimizers {

class Optimizer { // Base class
public:
    Optimizer(network::Network& net) : network(net) {};
    virtual ~Optimizer() = default;
    virtual void optimize() = 0;

protected:
    network::Network& network;
};


class ClusteringOptimizer : public Optimizer {
public:
    ClusteringOptimizer(network::Network& net) : Optimizer(net) {};
    
    void optimize(int maxIterations = 500, double minSpeed = 1e-5, double acceleration = 0.05);

    void optimize() override {
        optimize(500, 1e-5, 0.05);
    };
};


class SimulatedAnnealingOptimizer: public Optimizer {
public:
    SimulatedAnnealingOptimizer(network::Network& net) : Optimizer(net) {};
    
    void optimize(double initialTemp = 1000.0, double finalTemp = 1.0, double alpha = 0.95, int iterationsPerTemp = 100);

    void optimize() override {
        optimize(1000.0, 1.0, 0.95, 100);
    };
};


class AttractorOptimizer : public Optimizer {
public:
    AttractorOptimizer(network::Network& net) : Optimizer(net) {};

    void optimize(unsigned int maxIterations = 500);

    void optimize() override {
        optimize(500);
    };
};

} // namespace optimizers

#endif // OPTIMIZERS_HPP