#ifndef GPP_H
#define GPP_H

#include "network.hpp"

class GPP {
public:
    GPP(network::Network& net) : network(net) {}
    void solve();
private:
    network::Network& network;
};

#endif