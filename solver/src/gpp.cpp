#include "../include/gpp.h"

void GPP::solve() {
    network.addGateway({-45.868933, -67.512012, 15.0});
    network.addGateway({-45.836411, -67.477794, 15.0});
    network.connect();
}