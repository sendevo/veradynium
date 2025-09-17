#include "../include/network.hpp"

namespace network {

Network Network::fromFeatureCollection(const geojson::FeatureCollection& fc) {
    
    Network network;

    network.gateways.clear();
    network.end_devices.clear();

    for (size_t i = 0; i < fc.featureCount(); ++i) {
        const auto& feature = fc.getFeature(i);
        const auto& properties = feature.properties;

        if (feature.geometry_type != geojson::POINT) {
            // Later we can add support for polygons to limit coverage areas
            throw std::runtime_error("Invalid GeoJSON: only 'Point' geometry is supported.");
        }

        if (std::holds_alternative<geojson::Position>(feature.coords)) {
            const geojson::Position& pos = std::get<geojson::Position>(feature.coords);
            if (pos.size() < 2) {
                throw std::runtime_error("Invalid Point: must have at least [lon, lat]");
            }
            double lng = pos[0];
            double lat = pos[1];
            if (detail::require_string(properties, "type") == "end_device"){
                network.end_devices.push_back(EndDevice::parse_end_device(properties, lat, lng));
            }else{ 
                if (detail::require_string(properties, "type") == "gateway"){
                    network.gateways.push_back(Gateway::parse_gateway(properties, lat, lng));        
                } else {
                    throw std::runtime_error("Invalid GeoJSON: unknown feature type '" + detail::require_string(properties, "type") + "'");
                }
            }
        }else{
            throw std::runtime_error("Invalid GeoJSON: Point geometry must have Position coordinates.");
        }
    }

    return network;
}

Network Network::fromGeoJSON(const std::string& filepath) {
    auto fc = geojson::FeatureCollection::fromGeoJSON(filepath);
    Network network = Network::fromFeatureCollection(fc);
    return network;
};


void Network::connect() {
    // Parallelized version of connect using OpenMP
    // This function assigns each end device to the closest reachable gateway

    const size_t num_gws = gateways.size();

    if(num_gws == 0) { // No gateways available, all devices remain unassigned
        for (auto& ed : end_devices) {
            ed.assigned_gateway = nullptr;
        }
        return;
    }

    const size_t num_eds = end_devices.size();

    // Phase 1: parallel per-device search for best gateway
    std::vector<int> best_gw_idx(num_eds, -1);
    std::vector<double> best_dist(num_eds, INF);

    #pragma omp parallel for schedule(dynamic) // parallelize over end devices
    for (int j = 0; j < static_cast<int>(num_eds); ++j) {
        double minDist = INF;
        int best = -1;

        for (int i = 0; i < static_cast<int>(num_gws); ++i) {
            const auto& gw = gateways[i];
            const auto& ed = end_devices[j];

            // call combined LOS+distance (fast single call)
            bool los = elevation_grid.lineOfSight(gw.lat, gw.lng, ed.lat, ed.lng, gw.height, ed.height);
            double distance = elevation_grid.distance(gw.lat, gw.lng, ed.lat, ed.lng, gw.height, ed.height);

            if (los && distance < minDist) {
                minDist = distance;
                best = i;
            }
        }

        best_gw_idx[j] = best;
        best_dist[j] = minDist;
    }

    // Phase 2: clear and populate connections (sequential)
    disconnect();

    total_distance = 0.0;
    for (size_t j = 0; j < num_eds; ++j) {
        int best = best_gw_idx[j];
        if (best >= 0) {
            end_devices[j].assigned_gateway = &gateways[best];
            end_devices[j].distance_to_gateway = best_dist[j];
            gateways[best].connected_devices.push_back(&end_devices[j]);
            total_distance += best_dist[j];
        }
    }
};

void Network::disconnect() {
    for (auto& gw : gateways) gw.connected_devices.clear();
    for (auto& dev : end_devices) dev.assigned_gateway = nullptr;
};

geojson::FeatureCollection Network::toFeatureCollection() const {
    geojson::FeatureCollection feature_collection = geojson::FeatureCollection(); 


    double minLat = std::numeric_limits<double>::max();
    double minLng = std::numeric_limits<double>::max();
    double maxLat = std::numeric_limits<double>::lowest();
    double maxLng = std::numeric_limits<double>::lowest();

    // Add gateways
    for (const auto& gw : gateways) {
        geojson::Feature gw_location;
        gw_location.geometry_type = geojson::POINT;
        gw_location.properties = nlohmann::json{
            {"type", "gateway"},
            {"id", gw.id},
            {"height", gw.height}
        };
        gw_location.coords = geojson::Position{gw.lng, gw.lat};
        feature_collection.addFeature(gw_location);
        if (gw.lat < minLat) minLat = gw.lat;
        if (gw.lat > maxLat) maxLat = gw.lat;
        if (gw.lng < minLng) minLng = gw.lng;
        if (gw.lng > maxLng) maxLng = gw.lng;
    }

    // Add end devices and connections to assigned gateways (if any)
    for (const auto& ed : end_devices) {
        geojson::Feature ed_location;
        ed_location.geometry_type = geojson::POINT;
        ed_location.properties = nlohmann::json{
            {"type", "end_device"},
            {"id", ed.id},
            {"height", ed.height}
        };
        ed_location.coords = geojson::Position{ed.lng, ed.lat};
        if (ed.assigned_gateway) { // If assigned, add a line to the gateway
            ed_location.properties["assigned_gateway"] = ed.assigned_gateway->id;
            geojson::Feature connection;
            connection.geometry_type = geojson::LINESTRING;
            connection.properties = nlohmann::json{
                {"type", "connection"},
                {"from", ed.id},
                {"to", ed.assigned_gateway->id},
                {"distance", ed.distance_to_gateway}
            };
            connection.coords = geojson::LineString{
                geojson::Position{ed.lng, ed.lat}, 
                geojson::Position{ed.assigned_gateway->lng, ed.assigned_gateway->lat}
            };
            feature_collection.addFeature(connection);
        } else {
            ed_location.properties["assigned_gateway"] = nullptr;
        }
        feature_collection.addFeature(ed_location);
        if (ed.lat < minLat) minLat = ed.lat;
        if (ed.lat > maxLat) maxLat = ed.lat;
        if (ed.lng < minLng) minLng = ed.lng;
        if (ed.lng > maxLng) maxLng = ed.lng;
    }

    feature_collection.setBBox({minLng, minLat, maxLng, maxLat});

    feature_collection.setProperties({
        {"num_gateways", gateways.size()},
        {"num_end_devices", end_devices.size()},
        {"total_distance", total_distance}
    });

    return feature_collection;
};

void Network::printPlainText() const {
    std::cout << "Network Information:" << std::endl;
    std::cout << "Number of Gateways: " << gateways.size() << std::endl;
    for (const auto& gw : gateways) {
        std::cout << "  Gateway ID: " << gw.id 
                  << ", Lat: " << gw.lat 
                  << ", Lng: " << gw.lng 
                  << ", Height: " << gw.height << "m" << std::endl;
    }
    std::cout << "Number of End Devices: " << end_devices.size() << std::endl;
    for (const auto& ed : end_devices) {
        std::cout << "  End Device ID: " << ed.id 
                  << ", Lat: " << ed.lat 
                  << ", Lng: " << ed.lng 
                  << ", Height: " << ed.height << "m"
                  << ", Assigned Gateway: " 
                  << (ed.assigned_gateway ? ed.assigned_gateway->id : "None")
                  << std::endl;
    }
    std::cout << "Terrain Elevation Grid:" << std::endl;
    std::cout << "   Bounding Box:" << std::endl;
    std::cout << "      Upper right position: [" << elevation_grid.getBoundingBox()[0].lat << ", " << elevation_grid.getBoundingBox()[0].lng << "]" << std::endl;
    std::cout << "      Bottom left position: [" << elevation_grid.getBoundingBox()[2].lat << ", " << elevation_grid.getBoundingBox()[2].lng << "]" << std::endl;
    std::cout << "      Altitude range: [" << elevation_grid.getMinAltitude() << ", " << elevation_grid.getMaxAltitude() << "] meters" << std::endl;
    std::cout << "Total distance from end devices to assigned gateways: " << total_distance << " meters" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
};

void Network::printJSON() const {
    geojson::FeatureCollection feature_collection = toFeatureCollection();
    feature_collection.print();
};

void Network::print(global::PRINT_TYPE format) {
    connect();
    switch (format) {
        case global::PLAIN_TEXT:
            printPlainText();
            break;
        case global::JSON:
            printJSON();
            break;
        default:
            throw std::runtime_error("Unknown output format.");
    }
};

} // namespace network