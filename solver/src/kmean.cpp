#include "../include/kmean.hpp"

namespace kmean {

void KMeansOptimizer::optimize(int maxIterations, double epsilon) {
    // Not implemented yet
};

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
}

} // namespace kmean