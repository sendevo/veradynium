#include "../include/gpp.h"

namespace gpp {

void GPP::solve(GPP_METHOD method) {
    switch(method) {
        case GREEDY_RANDOM:
            greedyRandom();
            break;
        case GREEDY_DETERMINISTIC:
            throw std::runtime_error("GPP deterministic method not implemented yet.");
        default:
            throw std::runtime_error("Unknown GPP method.");
    }
};

void GPP::greedyRandom() {
    // Add gateways at random position inside bounding box until all end devices are connected
    // This method do not minimize energy consumption, only ensures full connectivity with a reasonable number of gateways
    const auto bbox = network.getBoundingBox();
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> lat_dist(bbox[1], bbox[3]);
    std::uniform_real_distribution<double> lng_dist(bbox[0], bbox[2]); 
    while(network.getConnectedEdCount() < network.getEndDevices().size()) {
        network.disconnect();
        double lat = lat_dist(rng);
        double lng = lng_dist(rng);
        network::Gateway& gw = network.addGateway({lat, lng, 15.0});
        network.connect();
        if(gw.connected_devices.empty()) {
            // Remove unconnected gateway
            auto& gateways = network.getGateways();
            gateways.pop_back();
            continue;
        }
        global::dbg << "GPP: Added gateway at (" << lat << ", " << lng << ") and connected network." << std::endl;
        global::dbg << "GPP: Connected end devices: " << network.getConnectedEdCount() << " / " << network.getEndDevices().size() << std::endl;
    }

    auto& gateways = network.getGateways();
    // Remove gateways one by one and check connectivity on each step
    for(size_t i = 0; i < gateways.size(); ) {
        network.disconnect();
        std::string gw_id = gateways[i].id;
        terrain::LatLngAlt pos = gateways[i].location;
        network.removeGateway(gw_id);
        network.connect();
        if(network.getConnectedEdCount() < network.getEndDevices().size()) {
            // Re-add gateway if network is disconnected
            network.addGateway(pos);
            i++; // only increment if gateway was not removed
        } else {
            global::dbg << "GPP: Removed gateway " << gw_id << ". Connected end devices: " << network.getConnectedEdCount() << " / " << network.getEndDevices().size() << std::endl;
        }
    }
}

} // namespace gpp