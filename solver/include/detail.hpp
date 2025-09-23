#pragma once
#ifndef DETAIL_HPP
#define DETAIL_HPP

#include "json.hpp"

/** 
 * 
 * @brief Helper functions for JSON parsing and validation
 * 
 */

namespace detail {

// Mandatory string field
inline std::string require_string(const nlohmann::json& obj, const std::string& key) {
    if(!obj.contains(key) || !obj[key].is_string()){
        throw std::runtime_error("Invalid JSON: missing or incorrect string field '" + key + "'");
    }
    return obj[key];
};

// Mandatory number field
inline double require_number(const nlohmann::json& obj, const std::string& key) {
    if(!obj.contains(key) || !obj[key].is_number()){
        throw std::runtime_error("Invalid JSON: missing or incorrect numeric field '" + key + "'");
    }
    return obj[key];
};

// Optional number field with default
inline double optional_number(const nlohmann::json& obj, const std::string& key, double def = 0.0) {
    if(!obj.contains(key)) return def;
    if(!obj[key].is_number()){
        throw std::runtime_error("Invalid JSON: incorrect numeric field '" + key + "'");
    }
    return obj[key];
};

} // namespace detail

#endif // DETAIL_HPP