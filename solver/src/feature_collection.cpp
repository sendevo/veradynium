#include "../include/feature_collection.hpp"


namespace geojson {

FeatureCollection FeatureCollection::fromGeoJSON(const std::string& filename) {
    FeatureCollection fc;

    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open GeoJSON file: " + filename);
    }

    json data;
    file >> data;

    if (data.value("type", "") != "FeatureCollection") {
        throw std::runtime_error("Invalid GeoJSON: root type is not FeatureCollection");
    }

    for (const auto& feature : data["features"]) {
        if (feature.value("type", "") != "Feature") {
            throw std::runtime_error("Invalid GeoJSON: feature missing or incorrect 'type'");
        }

        const auto& properties = feature.at("properties");
        const auto& geometry   = feature.at("geometry");

        if(geometry.value("type", "") != "Point") { // TODO: add support for other geometry types
            throw std::runtime_error("Invalid GeoJSON: only 'Point' geometries are supported.");
        }

        auto coords = geometry.at("coordinates");
        if(!coords.is_array() || coords.size() < 2) {
            throw std::runtime_error("Invalid GeoJSON: invalid coordinates.");
        }

        double lng = coords[0];
        double lat = coords[1];
        
        std::string type = detail::require_string(properties, "type");

        if(type == "gateway" || type == "end_device") {
            fc.addFeature(Feature(POINT, Position{lng, lat}, properties));
        } else {
            throw std::runtime_error("Invalid GeoJSON: unknown feature type '" + type + "'");
        }
    }

    // compute bbox if not present
    if(data.contains("bbox") && data["bbox"].is_array() && data["bbox"].size() == 4) {
        fc.bbox = data["bbox"].get<std::vector<double>>();
    } else {
        if(!fc.features.empty()) {
            double minX = std::get<Position>(fc.features[0].coords)[0];
            double minY = std::get<Position>(fc.features[0].coords)[1];
            double maxX = minX;
            double maxY = minY;
            for(const auto& feat : fc.features) {
                const auto& pos = std::get<Position>(feat.coords);
                if(pos[0] < minX) minX = pos[0];
                if(pos[1] < minY) minY = pos[1];
                if(pos[0] > maxX) maxX = pos[0];
                if(pos[1] > maxY) maxY = pos[1];
            }
            fc.bbox = {minX, minY, maxX, maxY};
        }
    }

    return fc;
}

void FeatureCollection::saveToFile(const std::string& filename) const {
    json data;
    data["type"] = "FeatureCollection";
    data["features"] = json::array();

    for (const auto& feature : features) {
        json feat;
        feat["type"] = "Feature";

        // Geometry
        json geom;
        switch (feature.geometry_type) {
            case POINT:
                geom["type"] = "Point";
                geom["coordinates"] = std::get<Position>(feature.coords);
                break;
            case LINESTRING:
                geom["type"] = "LineString";
                geom["coordinates"] = std::get<LineString>(feature.coords);
                break;
            case POLYGON:
                geom["type"] = "Polygon";
                geom["coordinates"] = std::get<Polygon>(feature.coords);
                break;
            case MULTIPOLYGON:
                geom["type"] = "MultiPolygon";
                geom["coordinates"] = std::get<MultiPolygon>(feature.coords);
                break;
            default:
                throw std::runtime_error("Unsupported geometry type for serialization");
        }
        feat["geometry"] = geom;

        // Properties
        feat["properties"] = feature.properties;

        data["features"].push_back(feat);
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + filename);
    }

    file << data.dump(4); // Pretty print with 4 spaces
};

void FeatureCollection::print() const {
    // Print JSON format to console
    std::cout << "{\n  \"type\": \"FeatureCollection\",\n  \"features\": [\n";
    for (size_t i = 0; i < features.size(); ++i) {
        const auto& feature = features[i];
        std::cout << "    {\n";
        std::cout << "      \"type\": \"Feature\",\n";
        std::cout << "      \"geometry\": {\n";
        switch (feature.geometry_type) {
            case POINT:
                std::cout << "        \"type\": \"Point\",\n";
                std::cout << "        \"coordinates\": " << json(std::get<Position>(feature.coords)).dump() << "\n";
                break;
            case LINESTRING:
                std::cout << "        \"type\": \"LineString\",\n";
                std::cout << "        \"coordinates\": " << json(std::get<LineString>(feature.coords)).dump() << "\n";
                break;
            case POLYGON:
                std::cout << "        \"type\": \"Polygon\",\n";
                std::cout << "        \"coordinates\": " << json(std::get<Polygon>(feature.coords)).dump() << "\n";
                break;
            case MULTIPOLYGON:
                std::cout << "        \"type\": \"MultiPolygon\",\n";
                std::cout << "        \"coordinates\": " << json(std::get<MultiPolygon>(feature.coords)).dump() << "\n";
                break;
            default:
                throw std::runtime_error("Unsupported geometry type for printing");
        }
        std::cout << "      },\n";
        std::cout << "      \"properties\": " << feature.properties.dump(6) << "\n";
        std::cout << "    }" << (i < features.size() - 1 ? "," : "") << "\n";
    }
    std::cout << "  ]\n";
    if(!properties.is_null()) {
        std::cout << ",  \"properties\": " << properties.dump(2) << "\n";
    }
    if(!bbox.empty()) {
        std::cout << ",  \"bbox\": " << json(bbox).dump() << "\n";
    }
    std::cout << "}\n";
}

} // namespace geojson