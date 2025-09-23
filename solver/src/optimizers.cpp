#include "../include/optimizers.hpp"

namespace optimizers {

void ClusteringOptimizer::optimize(int maxIterations, double minSpeed, double acceleration) {
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
        global::dbg << "ed," << j << "," << ed.location.lat << "," << ed.location.lng << "," << 0 << "\n";
    }

    // Try with k = 1, 2, ... until all devices are connected
    for (int k = 1; k <= maxGateways; ++k) {
        // reset
        network.gateways.clear();

        // place k random gateways
        for (int i = 0; i < k; i++) {
            terrain::LatLngAlt location = {disLat(gen), disLng(gen), 2.0};
            network.gateways.emplace_back(std::to_string(i), location);
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

                if (!network.end_devices.empty()) {
                    double best = INF;
                    for (auto& ed : network.end_devices) {
                        //double dist = network.elevation_grid.distance(gw.lat, gw.lng, ed.lat, ed.lng, gw.height, ed.height);
                        double dist = network.elevation_grid.haversine(gw.location.lat, gw.location.lng, ed.location.lat, ed.location.lng);
                        if (dist < best) {
                            best = dist;
                        }
                    }
                }

                if (gw.connected_devices.empty()) continue;

                std::vector<terrain::LatLngAlt> points;
                for(network::EndDevice* ed : gw.connected_devices) {
                    points.push_back(ed->location);
                }
                terrain::LatLngAlt cent = terrain::getCentroid(points);

                double newLat = gw.location.lat + acceleration * (cent.lat - gw.location.lat);
                double newLng = gw.location.lng + acceleration * (cent.lng - gw.location.lng);
                double latDiff = newLat - gw.location.lat;
                double lngDiff = newLng - gw.location.lng;
                double sqmove = latDiff*latDiff + lngDiff*lngDiff;

                gw.location.lat = newLat;
                gw.location.lng = newLng;

                connected_eds = network.connect();

                speeds[i] = sqmove;
                if (speeds[i] > minSpeed) allStable = false;

                // debug output
                global::dbg << "gw," << gw.id << "," << gw.location.lat << "," << gw.location.lng << "\n";
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

void SimulatedAnnealingOptimizer::optimize(double initialTemp, double finalTemp, double alpha, int iterationsPerTemp) {
    // bounding box of all end-devices
    std::vector<double> bbox = network.getBoundingBox();

    // random generator
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> disLat(bbox[1], bbox[3]);
    static std::uniform_real_distribution<> disLng(bbox[0], bbox[2]);
    static std::uniform_real_distribution<> disProb(0.0, 1.0);

    // initial solution: 1 gateway at random position
    network.gateways.clear();
    terrain::LatLngAlt location = {disLat(gen), disLng(gen), 2.0};
    network.gateways.emplace_back("0", location);
    unsigned int connected_eds = network.connect();
    unsigned int best_connected_eds = connected_eds;
    auto best_gateways = network.gateways;

    double temp = initialTemp;

    global::dbg << "type,id,lat,lng,iter,temp,connected\n";
    for (size_t j = 0; j < network.end_devices.size(); ++j) {
        auto& ed = network.end_devices[j];
        global::dbg << "ed," << j << "," << ed.location.lat << "," << ed.location.lng << "," << 0 << "," << temp << "," << connected_eds << "\n";
    }
    for (size_t j = 0; j < network.gateways.size(); ++j) {
        auto& gw = network.gateways[j];
        global::dbg << "gw," << gw.id << "," << gw.location.lat << "," << gw.location.lng << "," << 0 << "," << temp << "," << connected_eds << "\n";
    }

    int iter = 0;
    while (temp > finalTemp) {
        for (int iters = 0; iters < iterationsPerTemp; ++iters) {
            iter++;
            // create neighbor solution by moving one gateway to a random position
            int gw_to_move = gen() % network.gateways.size();
            terrain::LatLngAlt old_location = network.gateways[gw_to_move].location;
            terrain::LatLngAlt new_location = {disLat(gen), disLng(gen), 2.0};
            network.gateways[gw_to_move].location = new_location;
            connected_eds = network.connect();
            // if better, accept
            if (connected_eds > best_connected_eds) {
                best_connected_eds = connected_eds;
                best_gateways = network.gateways;
            }
            // if worse, accept with probability
            else if (connected_eds < best_connected_eds) {
                double prob = exp((connected_eds - best_connected_eds) / temp);
                double rnd = disProb(gen);
                if (rnd >= prob) {
                    // reject, revert change
                    network.gateways[gw_to_move].location = old_location;
                    connected_eds = network.connect();
                }
            }
            // debug output
            for (size_t j = 0; j < network.gateways.size(); ++j) {
                auto& gw = network.gateways[j];
                global::dbg << "gw," << gw.id << "," << gw.location.lat << "," << gw.location.lng << "," << iter << "," << temp << "," << connected_eds << "\n";
            }
        }
        temp *= alpha;
    }
    network.gateways = best_gateways;
    connected_eds = network.connect();
    if (connected_eds == network.end_devices.size()) {
        //global::dbg << "All devices connected with " << network.gateways.size() << " gateways." << std::endl;
        return;
    } else {
        //global::dbg << "Warning: Could not connect all devices even with max gateways." << std::endl;
    }
};

} // namespace optimizers