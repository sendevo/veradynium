#ifndef GPP_H
#define GPP_H

#include "network.hpp"

namespace gpp {

enum GPP_METHOD {
    GREEDY_RANDOM,
    GREEDY_DETERMINISTIC,
    GA
};

class GPP {
public:
    GPP(network::Network& net) : network(net) {}
    void solve(GPP_METHOD method);
private:
    network::Network& network;
    
    void greedyRandom();
};

} // namespace gpp

#endif