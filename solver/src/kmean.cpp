#include "../include/kmean.hpp"

namespace kmean {


terrain::LatLngAlt KMeansOptimizer::computeCentroid(const network::Gateway& gw) {
    // Computes the centroid of the connected devices to the gateway
    if (gw.connected_devices.empty()) {
        return {gw.lat, gw.lng, gw.height}; // No connected devices, return gateway position
    }

    double sumLat = 0.0;
    double sumLng = 0.0;
    for (auto d : gw.connected_devices) {
        sumLat += d->lat;
        sumLng += d->lng;
    }

    return { // centroid position
        sumLat / gw.connected_devices.size(), 
        sumLng / gw.connected_devices.size(), 
        gw.height
    };
};

void KMeansOptimizer::optimize(int maxIterations, double epsilon) {
  
    // Step 1: begin with a disconnected list of end-devices
    iterations = 0;
    network.disconnect();
    network.gateways.clear();

    // Random generator
    static std::random_device              rd;
    static std::mt19937                    gen(rd());
    static std::uniform_int_distribution<> dis(0, network.end_devices.size());
    
    // Add a new gateway and move it towards to its centroid until stabilization or disconnection
    const std::size_t index = dis(gen); // Pick random end-device

    global::dbg << "Selected index: " << index << std::endl;

    network::Gateway gw(
        std::to_string(network.gateways.size()),
        network.end_devices.at(index).lat, 
        network.end_devices.at(index).lng, 
        2.0);
    network.gateways.push_back(gw);

    global::dbg << "Gateway initial position: " << gw.lat << ", " << gw.lng << std::endl;
    global::dbg << "Connecting network..." << std::endl;

    network.connect();
    std::size_t connected_eds = gw.connected_devices.size();
    terrain::LatLngAlt cent = computeCentroid(network.gateways.at(-1));

    global::dbg << "Centroid position: " << cent.lat << ", " << cent.lng << std::endl;
    global::dbg << "Connected eds: " << connected_eds << std::endl;
    
    double speed = INF;
    while(speed >= epsilon){
        const double newLat = gw.lat - cent.lat*0.01;
        const double newLng = gw.lng - cent.lng*0.01;
        double speed = (newLng - gw.lat)*(newLat - gw.lat) + (newLng - gw.lng)*(newLng - gw.lng);
        gw.lat = newLat;
        gw.lng = newLng;
        network.connect();
        if(gw.connected_devices.size() < connected_eds)
            break;
        iterations++;
        global::dbg << "Iteration: " << iterations << ", Current speed: " << speed << std::endl;
    }

    global::dbg << "Gateway final position: " << gw.lat << ", " << gw.lng << std::endl;
};

} // namespace kmean