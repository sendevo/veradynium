#include "../include/network.hpp"

namespace network {

Network Network::fromJSON(const std::string& filepath) {
    Network network;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open JSON file: " + filepath << std::endl;
        exit(1);
    }

    nlohmann::json j;
    file >> j;

    if(!j.contains("type") || j["type"] != "FeatureCollection"){
        std::cerr << "Invalid GeoJSON: missing or incorrect 'type' field." << std::endl;
        exit(1);
    }

    if(j.contains("features") && j["features"].is_array()){
        for (const auto& feature : j["features"]) {
            if(!feature.contains("type") || feature["type"] != "Feature"){
                std::cerr << "Invalid GeoJSON: missing or incorrect 'type' field in feature." << std::endl;
                exit(1);
            }

            if(!feature.contains("properties") || !feature["properties"].is_object()){
                std::cerr << "Invalid GeoJSON: missing or incorrect 'properties' field in feature." << std::endl;
                exit(1);
            }

            if(!feature.contains("geometry") || !feature["geometry"].is_object()){
                std::cerr << "Invalid GeoJSON: missing or incorrect 'geometry' field in feature." << std::endl;
                exit(1);
            }

            const auto& properties = feature["properties"];
            const auto& geometry = feature["geometry"];

            if(!geometry.contains("type") || geometry["type"] != "Point"){
                std::cerr << "Invalid GeoJSON: only 'Point' geometries are supported." << std::endl;
                exit(1);
            }

            if(!geometry.contains("coordinates") || !geometry["coordinates"].is_array() || geometry["coordinates"].size() < 2){
                std::cerr << "Invalid GeoJSON: missing or incorrect 'coordinates' field in geometry." << std::endl;
                exit(1);
            }

            double lng = geometry["coordinates"][0];
            double lat = geometry["coordinates"][1];

            if(!properties.contains("type") || !properties["type"].is_string()){
                std::cerr << "Invalid GeoJSON: missing or incorrect 'type' field in properties." << std::endl;
                exit(1);
            }

            std::string type = properties["type"];
            if(type == "gateway"){
                if(!properties.contains("id") || !properties["id"].is_string()){
                    std::cerr << "Invalid GeoJSON: missing or incorrect 'id' field for gateway." << std::endl;
                    exit(1);
                }
                if(!properties.contains("height") || !properties["height"].is_number()){
                    std::cerr << "Invalid GeoJSON: missing or incorrect 'height' field for gateway." << std::endl;
                    exit(1);
                }
                std::string id = properties["id"];
                double height = properties["height"];
                network.gateways.emplace_back(id, lat, lng, height);
            }else{
                if(type == "end_device"){
                    if(!properties.contains("id") || !properties["id"].is_string()){
                        std::cerr << "Invalid GeoJSON: missing or incorrect 'id' field for end device." << std::endl;
                        exit(1);
                    }
                    if(!properties.contains("height") || !properties["height"].is_number()){
                        std::cerr << "Invalid GeoJSON: incorrect 'height' field for end device." << std::endl;
                        exit(1);
                    }
                    std::string id = properties["id"];
                    double height = properties["height"];
                    network.end_devices.emplace_back(id, lat, lng, height);
                } else {
                    std::cerr << "Invalid GeoJSON: unknown feature type '" << type << "'." << std::endl;
                    exit(1);
                }
            }
        }
    } else {
        std::cerr << "Invalid GeoJSON: missing or incorrect 'features' field." << std::endl;
        exit(1);
    }

    return network;
}

} // namespace network