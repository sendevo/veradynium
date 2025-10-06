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

            const Node node = Node::parse(properties, pos[1], pos[0], &network.elevation_grid); // lat, lng
            if (detail::require_string(properties, "type") == "end_device"){
                network.end_devices.push_back(EndDevice(node.id, node.location, &network.elevation_grid));
            }else{ 
                if (detail::require_string(properties, "type") == "gateway"){
                    network.gateways.push_back(Gateway(node.id, node.location, &network.elevation_grid));
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

void Network::addGateway(terrain::LatLngAlt pos) {
    std::string new_id = global::generate_uuid();
    gateways.push_back(Gateway(new_id, pos, &elevation_grid));
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

    // Parallel per-device search for best gateway
    std::vector<int> best_gw_idx(num_eds, -1);
    std::vector<double> best_dist(num_eds, DBL_MAX);


    #pragma omp parallel for schedule(dynamic) // parallelize over end devices
    for (int j = 0; j < static_cast<int>(num_eds); ++j) {
        double minDist = DBL_MAX;
        int best = -1;

        for (int i = 0; i < static_cast<int>(num_gws); ++i) {
            const auto& gw = gateways[i];
            const auto& ed = end_devices[j];

            bool los = gw.lineOfSightTo(ed);
            //const double distance = gw.distanceTo(ed); // Equirectangular distance
            const double distance = elevation_grid.squaredDistance(gw.location, ed.location); // Squared distance for efficiency

            if (los && distance < minDist) {
                minDist = distance;
                best = i;
            }
        }

        // Use MAX_RANGE; -> if use equirectangular distance
        if(minDist < MAX_RANGE_SQUARED){ // Only consider connections within maximum range
            best_gw_idx[j] = best;
            best_dist[j] = minDist;
        }
    }

    // Reset pointers and connected_eds_cnt
    disconnect();

    for (size_t j = 0; j < num_eds; ++j) {
        int best = best_gw_idx[j];
        if (best >= 0) {
            end_devices[j].assigned_gateway = &gateways[best];
            gateways[best].connected_devices.push_back(&end_devices[j]);
            connected_eds_cnt++;
        }
    }
};

void Network::disconnect() {
    for (auto& gw : gateways) gw.connected_devices.clear();
    for (auto& dev : end_devices) dev.assigned_gateway = nullptr;
    connected_eds_cnt = 0;
};

double Network::computeTotalDistance() const {
    double total_distance = 0.0;
    for (const auto& ed : end_devices) {
        if (ed.assigned_gateway) {
            total_distance += ed.distanceTo(*ed.assigned_gateway);
        }
    }
    return total_distance;
};

std::vector<size_t> Network::computeDistanceHistogram() const {
    std::vector<size_t> histogram((MAX_RANGE / DISTANCE_HISTOGRAM_BIN_SIZE) + 1, 0);
    for (const auto& ed : end_devices) {
        if (ed.assigned_gateway) {
            double distance = ed.distanceTo(*ed.assigned_gateway);
            size_t bin = static_cast<size_t>(distance / DISTANCE_HISTOGRAM_BIN_SIZE);
            if (bin < histogram.size()) {
                histogram[bin] += 1;
            }
        }
    }
    return histogram;
};

geojson::FeatureCollection Network::toFeatureCollection() const {
    geojson::FeatureCollection feature_collection = geojson::FeatureCollection(); 


    double minLat = std::numeric_limits<double>::max();
    double minLng = std::numeric_limits<double>::max();
    double maxLat = std::numeric_limits<double>::lowest();
    double maxLng = std::numeric_limits<double>::lowest();

    // Add gateways
    for (const auto& gw : gateways) {
        std::vector<std::string> connected_device_ids;
        for(const auto& dev : gw.connected_devices) {
            connected_device_ids.push_back(dev->id);
        }
        geojson::Feature gw_location;
        gw_location.geometry_type = geojson::POINT;
        gw_location.properties = nlohmann::json{
            {"type", "gateway"},
            {"id", gw.id},
            {"height", gw.location.alt},
            {"connected_devices", connected_device_ids}
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
                {"distance", ed.distanceTo(*ed.assigned_gateway)}
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
        {"total_distance", computeTotalDistance()},
        {"distance_histogram_bin_size", DISTANCE_HISTOGRAM_BIN_SIZE},
        {"distance_histogram", computeDistanceHistogram()},
        {"connected_end_devices", static_cast<int>(end_devices.size() - std::count_if(end_devices.begin(), end_devices.end(), [](const EndDevice& ed){ return ed.assigned_gateway == nullptr; }))},
        {"disconnected_end_devices", static_cast<int>(std::count_if(end_devices.begin(), end_devices.end(), [](const EndDevice& ed){ return ed.assigned_gateway == nullptr; }))},
        {"elevation_grid", {
            {"bounding_box", {
                {"upper_right", {elevation_grid.getBoundingBox()[0].lat, elevation_grid.getBoundingBox()[0].lng}},
                {"bottom_left", {elevation_grid.getBoundingBox()[2].lat, elevation_grid.getBoundingBox()[2].lng}}
            }},
            {"altitude_range", {elevation_grid.getMinAltitude(), elevation_grid.getMaxAltitude()}}
        }},
        {"max_connection_distance", MAX_RANGE},
        {"network_bbox", {
            {"upper_right", {maxLat, maxLng}},
            {"bottom_left", {minLat, minLng}}
        }}
    });

    return feature_collection;
};

void Network::printPlainText() const {
    std::cout << std::endl << "Network Information:" << std::endl << "----------------------------------------" << std::endl;
    std::cout << "Number of Gateways: " << gateways.size() << std::endl;
    for (const auto& gw : gateways) {
        std::cout << "  Gateway ID: " << gw.id << std::endl
                  << "    Lat: " << gw.location.lat << std::endl
                  << "    Lng: " << gw.location.lng << std::endl
                  << "    Height: " << gw.location.alt << "m" << std::endl
                  << "    Connected End Devices: " << gw.connected_devices.size() << std::endl;
    }
    std::cout << "Number of End Devices: " << end_devices.size() << std::endl;
    for (const auto& ed : end_devices) {
        std::cout << "  End Device ID: " << ed.id << std::endl
                  << "    Lat: " << ed.location.lat << std::endl
                  << "    Lng: " << ed.location.lng << std::endl 
                  << "    Height: " << ed.location.alt << "m" << std::endl
                  << "    Assigned Gateway: " 
                  << (ed.assigned_gateway ? ed.assigned_gateway->id : "None") << std::endl;
        std::cout << "    Distance to Gateway: " << (ed.assigned_gateway ? std::to_string(ed.distanceTo(*ed.assigned_gateway)) + " meters" : "N/A")
                  << std::endl;
    }
    std::cout << "Terrain Elevation Grid:" << std::endl;
    std::cout << "   Bounding Box:" << std::endl;
    std::cout << "      Upper right position: [" << elevation_grid.getBoundingBox()[0].lat << ", " << elevation_grid.getBoundingBox()[0].lng << "]" << std::endl;
    std::cout << "      Bottom left position: [" << elevation_grid.getBoundingBox()[2].lat << ", " << elevation_grid.getBoundingBox()[2].lng << "]" << std::endl;
    std::cout << "      Altitude range: [" << elevation_grid.getMinAltitude() << ", " << elevation_grid.getMaxAltitude() << "] meters" << std::endl;
    std::cout << "Total distance from end devices to assigned gateways: " << computeTotalDistance() << " meters" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    // Print distance matrix
    std::cout << "Distance Matrix (meters):" << std::endl;
    std::cout << "end-device-id";
    for(size_t g = 0; g < gateways.size(); g++) {
        std::cout << "," << gateways[g].id;
    }
    std::cout << std::endl;
    for(size_t e = 0; e < end_devices.size(); e++) {
        std::cout << end_devices[e].id;
        for(size_t g = 0; g < gateways.size(); g++) {
            double dist = elevation_grid.equirectangularDistance(
                getEndDeviceLocation(e),
                getGatewayLocation(g)
            );
            if(elevation_grid.lineOfSight(
                getEndDeviceLocation(e),
                getGatewayLocation(g)
            )) {
                std::cout << "," << dist;
            } else {
                std::cout << "," << -1.0;
            }
        }
        std::cout << std::endl;
    }
};

void Network::printJSON() const {
    geojson::FeatureCollection feature_collection = toFeatureCollection();
    feature_collection.print();
};

void Network::print(global::PRINT_TYPE format) {
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