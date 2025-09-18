#include "../include/kmean.hpp"

namespace kmean {


terrain::LatLngAlt KMeansOptimizer::computeCentroid(const network::Gateway& gw, double fallbackLat, double fallbackLng) {
    // Computes the centroid of the connected devices to the gateway
    if (gw.connected_devices.empty()) {
        return {fallbackLat, fallbackLng, gw.height}; // If no connected devices, return fallback position
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

void KMeansOptimizer::optimize(int maxIterations, double minSpeed, double acceleration) {
  
    // Begin with a disconnected list of end-devices
    network.gateways.clear();

    // Random generator
    static std::random_device              rd;
    static std::mt19937                    gen(rd());
    static std::uniform_int_distribution<> dis(0, network.end_devices.size()-1);
    std::vector<double> bbox = network.getBoundingBox();
    
    unsigned int connected_eds = 0;
    std::vector<double> speeds;
    while(connected_eds < network.end_devices.size() && network.gateways.size() < network.end_devices.size()) {
        // Pick a random position within the bounding box
        double lat = bbox[1] + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX/(bbox[3]-bbox[1])));
        double lng = bbox[0] + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX/(bbox[2]-bbox[0])));

        network::Gateway gw(std::to_string(network.gateways.size()), lat, lng, 2.0);
        network.gateways.push_back(gw);
        speeds.push_back(INF);
        connected_eds = network.connect();

        global::dbg << "Added gateway at: " << lat << ", " << lng << " - Connected devices: " << connected_eds << "/" << network.end_devices.size() << std::endl;
        global::dbg << "Total gateways: " << network.gateways.size() << std::endl;
        
        for(unsigned int i = 0; i < network.gateways.size(); i++) {
            auto& gw = network.gateways[i];
            // Pick closest end-device as fallback position
            double fallbackLat = 0.0;
            double fallbackLng = 0.0;
            if(!gw.connected_devices.empty()) {
                double minDist = std::numeric_limits<double>::max();
                for(auto d : gw.connected_devices) {
                    double dist = network.elevation_grid.haversine(gw.lat, gw.lng, d->lat, d->lng);
                    if(dist < minDist) {
                        minDist = dist;
                        fallbackLat = d->lat;
                        fallbackLng = d->lng;
                    }
                }
            } else {
                if(!network.end_devices.empty()) {
                    // If no connected devices, pick a random end-device
                    int idx = dis(gen);
                    fallbackLat = network.end_devices[idx].lat;
                    fallbackLng = network.end_devices[idx].lng;
                }
            }
            global::dbg << "Optimizing gateway: " << gw.id << " - Connected devices: " << gw.connected_devices.size() << std::endl;
            terrain::LatLngAlt cent = computeCentroid(gw, fallbackLat, fallbackLng);
            while(speeds[i] > minSpeed) {
                // Move gateway slightly towards centroid
                const double newLat = gw.lat + acceleration * (cent.lat - gw.lat);
                const double newLng = gw.lng + acceleration * (cent.lng - gw.lng);
                const double latDiff = newLat - gw.lat;
                const double lngDiff = newLng - gw.lng;
                speeds[i] = latDiff*latDiff + lngDiff*lngDiff;
                gw.lat = newLat;
                gw.lng = newLng;
                unsigned int prev_connected_eds = gw.connected_devices.size();
                network.connect();
                if(gw.connected_devices.size() < prev_connected_eds){ // If we lost connections, revert movement and add a new gateway
                    gw.lat -= latDiff;
                    gw.lng -= lngDiff;
                    network.connect();
                    global::dbg << "Lost connections, reverting movement and adding new gateway." << std::endl;
                    break;
                }
            }
        }
    }
    
};

} // namespace kmean