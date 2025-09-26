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

double calculateVelocityMagnitude(const std::vector<terrain::LatLngAlt>& velocities) {
    double total_velocity = 0.0;
    for (const auto& vel : velocities) {
        total_velocity += std::sqrt(vel.lat*vel.lat + vel.lng*vel.lng);
    }
    return total_velocity / velocities.size();
}

void AttractorOptimizer::optimize(unsigned int maxIterations) {

    std::vector<double> bbox = network.getBoundingBox();

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> disLat(bbox[1], bbox[3]);
    static std::uniform_real_distribution<> disLng(bbox[0], bbox[2]);

    for(unsigned int iter = 0; iter < maxIterations; iter++){
        network.connect(); // This disconnects before connecting

        // Not connected end-devices count
        const std::size_t nced = network.getEndDevices().size() - network.getConnectedEdCount();

        if(nced == 0)
            break;
        
        std::vector<terrain::LatLngAlt> velocities(network.getGateways().size());

        #pragma omp parallel for schedule(dynamic) // parallelize over gateways
        for(std::size_t g = 0; g < network.getGateways().size(); g++) {
            terrain::LatLngAlt total_force = {0.0, 0.0, 0.0}; // Thread-local
            
            for(std::size_t e = 0; e < network.getEndDevices().size(); e++) {
                terrain::LatLngAlt vec = network.getEndDeviceLocation(e) - network.getGatewayLocation(g);
                terrain::LatLngAlt acc;
                
                if(network.getEndDevices()[e].assigned_gateway == &network.getGateways()[g]) { // Connected to this gateway
                    acc = vec / network.getEndDevices().size(); 
                } else { // Not connected to this gateway
                    acc = vec / nced; 
                }
                total_force += acc;
            }
            
            velocities[g] = total_force;
            network.translateGateway(g, velocities[g]);
        }

        double avg_velocity = calculateVelocityMagnitude(velocities);
        
        if(avg_velocity < 0.00001)
            break;
    }

    return;
};

} // namespace optimizers