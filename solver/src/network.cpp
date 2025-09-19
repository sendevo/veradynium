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

            const Node node = Node::parse(properties, pos[1], pos[0]);
            if (detail::require_string(properties, "type") == "end_device"){
                network.end_devices.push_back(EndDevice(node.id, node.location));
            }else{ 
                if (detail::require_string(properties, "type") == "gateway"){
                    network.gateways.push_back(Gateway(node.id, node.location));
                } else {
                    throw std::runtime_error("Invalid GeoJSON: unknown feature type '" + detail::require_string(properties, "type") + "'");
                }
            }
        }else{
            throw std::runtime_error("Invalid GeoJSON: Point geometry must have Position coordinates.");
        }
    }

    network.bbox = fc.getBBox();

    return network;
}

Network Network::fromGeoJSON(const std::string& filepath) {
    auto fc = geojson::FeatureCollection::fromGeoJSON(filepath);
    Network network = Network::fromFeatureCollection(fc);
    return network;
};


unsigned int Network::connect() {
    // Parallelized version of connect using OpenMP
    // This function assigns each end device to the closest reachable gateway

    const size_t num_gws = gateways.size();

    if(num_gws == 0) { // No gateways available, all devices remain unassigned
        for (auto& ed : end_devices) {
            ed.assigned_gateway = nullptr;
        }
        return 0;
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
            bool los = elevation_grid.lineOfSight(gw.location, ed.location);
            double distance = elevation_grid.distance(gw.location, ed.location);

            if (los && distance < minDist) {
                minDist = distance;
                best = i;
            }
        }

        if(minDist < MAX_DISTANCE){ // Only consider connections within MAX_DISTANCE
            best_gw_idx[j] = best;
            best_dist[j] = minDist;
        }
    }

    // Phase 2: clear and populate connections (sequential)
    disconnect();

    unsigned int connected_count = 0;
    total_distance = 0.0;
    for (size_t j = 0; j < num_eds; ++j) {
        int best = best_gw_idx[j];
        if (best >= 0) {
            end_devices[j].assigned_gateway = &gateways[best];
            end_devices[j].distance_to_gateway = best_dist[j];
            gateways[best].connected_devices.push_back(&end_devices[j]);
            total_distance += best_dist[j];
            connected_count++;
        }
    }

    return connected_count;
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
            {"height", gw.location.alt}
        };
        gw_location.coords = geojson::Position{gw.location.lng, gw.location.lat};
        feature_collection.addFeature(gw_location);
        if (gw.location.lat < minLat) minLat = gw.location.lat;
        if (gw.location.lat > maxLat) maxLat = gw.location.lat;
        if (gw.location.lng < minLng) minLng = gw.location.lng;
        if (gw.location.lng > maxLng) maxLng = gw.location.lng;
    }

    // Add end devices and connections to assigned gateways (if any)
    for (const auto& ed : end_devices) {
        geojson::Feature ed_location;
        ed_location.geometry_type = geojson::POINT;
        ed_location.properties = nlohmann::json{
            {"type", "end_device"},
            {"id", ed.id},
            {"height", ed.location.alt}
        };
        ed_location.coords = geojson::Position{ed.location.lng, ed.location.lat};
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
                geojson::Position{ed.location.lng, ed.location.lat}, 
                geojson::Position{ed.assigned_gateway->location.lng, ed.assigned_gateway->location.lat}
            };
            feature_collection.addFeature(connection);
        } else {
            ed_location.properties["assigned_gateway"] = nullptr;
        }
        feature_collection.addFeature(ed_location);
        if (ed.location.lat < minLat) minLat = ed.location.lat;
        if (ed.location.lat > maxLat) maxLat = ed.location.lat;
        if (ed.location.lng < minLng) minLng = ed.location.lng;
        if (ed.location.lng > maxLng) maxLng = ed.location.lng;
    }

    feature_collection.setBBox({minLng, minLat, maxLng, maxLat});

    feature_collection.setProperties({
        {"num_gateways", gateways.size()},
        {"num_end_devices", end_devices.size()},
        {"total_distance", total_distance},
        {"connected_end_devices", static_cast<int>(end_devices.size() - std::count_if(end_devices.begin(), end_devices.end(), [](const EndDevice& ed){ return ed.assigned_gateway == nullptr; }))},
        {"disconnected_end_devices", static_cast<int>(std::count_if(end_devices.begin(), end_devices.end(), [](const EndDevice& ed){ return ed.assigned_gateway == nullptr; }))},
        {"elevation_grid", {
            {"bounding_box", {
                {"upper_right", {elevation_grid.getBoundingBox()[0].lat, elevation_grid.getBoundingBox()[0].lng}},
                {"bottom_left", {elevation_grid.getBoundingBox()[2].lat, elevation_grid.getBoundingBox()[2].lng}}
            }},
            {"altitude_range", {elevation_grid.getMinAltitude(), elevation_grid.getMaxAltitude()}}
        }},
        {"max_connection_distance", MAX_DISTANCE},
        {"network_bbox", {
            {"upper_right", {maxLat, maxLng}},
            {"bottom_left", {minLat, minLng}}
        }}
    });

    return feature_collection;
};

void Network::printPlainText() const {
    std::cout << "Network Information:" << std::endl;
    std::cout << "Number of Gateways: " << gateways.size() << std::endl;
    for (const auto& gw : gateways) {
        std::cout << "  Gateway ID: " << gw.id 
                  << ", Lat: " << gw.location.lat 
                  << ", Lng: " << gw.location.lng 
                  << ", Height: " << gw.location.alt << "m" << std::endl;
    }
    std::cout << "Number of End Devices: " << end_devices.size() << std::endl;
    for (const auto& ed : end_devices) {
        std::cout << "  End Device ID: " << ed.id 
                  << ", Lat: " << ed.location.lat 
                  << ", Lng: " << ed.location.lng 
                  << ", Height: " << ed.location.alt << "m"
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