export const geoJSONToLeaflet = geoJSON => {
    const newGeoJSON = JSON.parse(JSON.stringify(geoJSON));
    if(newGeoJSON.features.isArray){
        newGeoJSON.features.forEach(feature => {
            const coordinates = feature.geometry.coordinates;
            if (Array.isArray(coordinates[0])) { // Nested array (e.g., Polygon)
                coordinates.forEach(coord => coord.reverse());
            } else { // Point
                feature.geometry.coordinates.reverse();
            }
        });
    }
    return newGeoJSON;
};

export const isValidGeoJSON = geoJSON => {
    try {
        if(geoJSON.type != "FeatureCollection")
            return false;
        if(!Array.isArray(geoJSON.features))
            return false;
        if(!geoJSON.features.every(feature => feature.type === "Feature"))
            return false;
        if(!geoJSON.features.every(feature => feature.geometry))
            return false;
        if(!geoJSON.features.every(feature => feature.geometry.type))
            return false;
        if(!geoJSON.features.every(feature => feature.geometry.coordinates))
            return false;
        return true;
    } catch (error) {
        return false;
    }
}

export const removeSlash = path => path.startsWith('/') ? path.slice(1) : path;