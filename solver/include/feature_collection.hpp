#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <stdexcept>
#include <variant>

#include "json.hpp"
#include "detail.hpp"


/**
 * @brief Class to represent and access a GeoJSON FeatureCollection
 */


using json = nlohmann::json;

namespace geojson {

enum GEOMETRY_TYPE { 
    POINT, 
    LINESTRING, 
    POLYGON, 
    MULTIPOINT, 
    MULTILINESTRING, 
    MULTIPOLYGON, 
    GEOMETRYCOLLECTION 
};

using Position    = std::vector<double>; // [lng, lat] or [lng, lat, alt]
using LineString  = std::vector<Position>;
using Polygon     = std::vector<LineString>;
using MultiPolygon= std::vector<Polygon>;   
using GeometryData = std::variant<
    Position,
    LineString,
    Polygon,
    MultiPolygon
>;

class Feature {
public:
    Feature() = default;

    Feature(const GEOMETRY_TYPE& geomType,
            const GeometryData& coordinates,
            const json& properties)
        : geometry_type(geomType), coords(coordinates), properties(properties) {}

    GEOMETRY_TYPE geometry_type;
    GeometryData coords;
    json properties;
};

class FeatureCollection {
public:
    FeatureCollection() = default;

    static FeatureCollection fromGeoJSON(const std::string& filename);
    void saveToFile(const std::string& filename) const;

    inline size_t featureCount() const { return features.size(); }
    inline Feature getFeature(size_t index) const { return features.at(index); }
    inline void addFeature(Feature feature) { features.push_back(feature); }

    void print() const;

private:
    std::vector<Feature> features;
    std::vector<double> bbox;
};

} // namespace geojson