#include "../include/optimizers.hpp"

namespace optimizers {

void ClusteringOptimizer::optimize(int maxIterations, double minSpeed, double acceleration) {
    // TODO
    return;
};

void SimulatedAnnealingOptimizer::optimize(double initialTemp, double finalTemp, double alpha, int iterationsPerTemp) {
    // TODO
    return;
};

void AttractorOptimizer::optimize(unsigned int maxIterations) {

    std::vector<double> bbox = network.getBoundingBox();

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> disLat(bbox[1], bbox[3]);
    static std::uniform_real_distribution<> disLng(bbox[0], bbox[2]);

    std::vector<terrain::LatLngAlt> velocities(network.getGateways().size());

    for(unsigned int iter = 0; iter < maxIterations; iter++){
        network.connect();

        // Not connected end-devices count
        const std::size_t nced = network.getEndDevices().size() - network.getConnectedEdCount();

        if(nced == 0)
            break;

        for(std::size_t g = 0; g < network.getGateways().size(); g++) {
            for(std::size_t e = 0; e < network.getEndDevices().size(); e++) {
                if(network.getEndDevices()[e].assigned_gateway != nullptr){ // If connected -> weak force
                    velocities[g] += ((network.getEndDevices()[e].location - network.getGateways()[g].location)/network.getEndDevices().size());
                }else{ // Connected -> strong force
                    velocities[g] += ((network.getEndDevices()[e].location - network.getGateways()[g].location)/nced);
                } 
            }
            network.getGateways()[g].location += velocities[g];
        }

        // TODO: measure stagnation
        // Add gateway at random position once stagnated
    }

    return;
};

} // namespace optimizers