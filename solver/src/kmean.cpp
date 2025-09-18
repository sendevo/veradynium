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
    // bounding box of all end-devices
    std::vector<double> bbox = network.getBoundingBox();

    // random generator
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> disLat(bbox[1], bbox[3]);
    static std::uniform_real_distribution<> disLng(bbox[0], bbox[2]);

    const int maxGateways = network.end_devices.size(); // safe upper bound

    global::dbg << "type,id,lat,lng,iter\n";
    for (size_t j = 0; j < network.end_devices.size(); ++j) {
        auto& ed = network.end_devices[j];
        global::dbg << "ed," << j << "," << ed.lat << "," << ed.lng << "," << 0 << "\n";
    }

    // Try with k = 1, 2, ... until all devices are connected
    for (int k = 1; k <= maxGateways; ++k) {
        // reset
        network.gateways.clear();

        // place k random gateways
        for (int i = 0; i < k; i++) {
            double lat = disLat(gen);
            double lng = disLng(gen);
            network.gateways.emplace_back(std::to_string(i), lat, lng, 2.0);
        }

        unsigned int connected_eds = network.connect();
        if (connected_eds == network.end_devices.size()) {
            //global::dbg << "All devices connected with " << k << " gateways (no optimization)." << std::endl;
            return;
        }

        // per-gateway speeds
        std::vector<double> speeds(k, INF);

        // iterate centroid updates
        for (int iter = 0; iter < maxIterations; ++iter) {
            bool allStable = true;

            for (int i = 0; i < k; i++) { // for each gateway
                auto& gw = network.gateways[i];

                // fallback: nearest device
                double fallbackLat = 0.0, fallbackLng = 0.0;
                if (!network.end_devices.empty()) {
                    double best = INF;
                    for (auto& ed : network.end_devices) {
                        //double dist = network.elevation_grid.distance(gw.lat, gw.lng, ed.lat, ed.lng, gw.height, ed.height);
                        double dist = network.elevation_grid.haversine(gw.lat, gw.lng, ed.lat, ed.lng);
                        if (dist < best) {
                            best = dist;
                            fallbackLat = ed.lat;
                            fallbackLng = ed.lng;
                        }
                    }
                }

                terrain::LatLngAlt cent = computeCentroid(gw, fallbackLat, fallbackLng);

                if (gw.connected_devices.empty()) continue;

                double newLat = gw.lat + acceleration * (cent.lat - gw.lat);
                double newLng = gw.lng + acceleration * (cent.lng - gw.lng);
                double latDiff = newLat - gw.lat;
                double lngDiff = newLng - gw.lng;
                double sqmove = latDiff*latDiff + lngDiff*lngDiff;

                gw.lat = newLat;
                gw.lng = newLng;

                connected_eds = network.connect();

                speeds[i] = sqmove;
                if (speeds[i] > minSpeed) allStable = false;

                // debug output
                global::dbg << "gw," << gw.id << "," << gw.lat << "," << gw.lng << "," << iterations << "\n";
                // Compile and test using:
                // (before -->) source ../server/venv/bin/activate
                // make && ./bin/solver -f ../data/topography/data/topography_nasa.csv -g ../data/network/network_20x16.json --dbg >> ./tests/movs.csv && cd tests && python3 test.py && cd ..
                // (after -->) deactivate


            }

            if (allStable) break;
        }

        connected_eds = network.connect();
        if (connected_eds == network.end_devices.size()) {
            //global::dbg << "All devices connected with " << k << " gateways." << std::endl;
            return;
        }
    }

    //global::dbg << "Warning: Could not connect all devices even with max gateways." << std::endl;
};


} // namespace kmean