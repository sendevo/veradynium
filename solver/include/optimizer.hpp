#pragma once
#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP

#include "network.hpp"

/**
 * 
 * @brief Optimization algorithms for the Gateway Placement Problem.
 * 
 */

namespace optimizer {

class Optimizer { // Base class
public:
    Optimizer(network::Network& net) : network(net) {};
    virtual ~Optimizer() = default;
    virtual void optimize() = 0;

protected:
    network::Network& network;
};

} // namespace optimizer

#endif // OPTIMIZER_HPP