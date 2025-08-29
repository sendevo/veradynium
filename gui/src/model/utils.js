export const removeSlash = path => path.startsWith('/') ? path.slice(1) : path;

export const isValidGeoJSON = geoJSON => {
    try {
        if (!geoJSON || geoJSON.type !== "FeatureCollection") return false;
        if (!Array.isArray(geoJSON.features)) return false;

        return geoJSON.features.every(feature => {
            if (!feature || feature.type !== "Feature") return false;
            const geom = feature.geometry;
            if (!geom || !geom.type || !geom.coordinates) return false;
            return true;
        });
    } catch {
        return false;
    }
};

export const getFileFormat = content => {
    // Detects if content is JSON, GeoJSON, CSV or unknown
    const trimmed = content.trim();

    // Quick check for JSON
    if (trimmed.startsWith("{") || trimmed.startsWith("[")) {
        try {
            const jsn = JSON.parse(trimmed); // will throw if not valid JSON
            if (isValidGeoJSON(jsn))
                return "geojson";
            else
                return "json";
        } catch (e) {
            return "unknown";
        }
    }

    // CSV heuristic: lines separated by newline, columns separated by comma/semicolon/tab
    const lines = trimmed.split(/\r?\n/);

    if (lines.length > 1) {
        const firstLine = lines[0];
        const delimiter = firstLine.includes(",") ? "," :
            firstLine.includes(";") ? ";" :
            firstLine.includes("\t") ? "\t" : null;

        if (delimiter) {
            const columns = firstLine.split(delimiter).length;
            const secondLine = lines[1].split(delimiter).length;

            if (columns > 1 && columns === secondLine) {
                return "csv";
            }
        }
    }

    return "unknown";
}

export const csvToArray = (csvString, withHeaders = true, delimiter = ",") => {
    const lines = csvString.trim().split(/\r?\n/);
    if (lines.length === 0) return [];

    if (withHeaders) {
        const headers = lines[0].split(delimiter).map(h => h.trim());
        return lines.slice(1).map(line => {
            const values = line.split(delimiter).map(v => {
                const trimmed = v.trim();
                return trimmed !== "" && !isNaN(trimmed) ? Number(trimmed) : trimmed;
            });
            const entry = {};
            headers.forEach((header, index) => {
                entry[header] = values[index] !== undefined ? values[index] : null;
            });
            return entry;
        });
    } else {
        // Return 2D array (matrix) skipping first line
        return lines.slice(1).map(line =>
            line.split(delimiter).map(v => {
                const trimmed = v.trim();
                return trimmed !== "" && !isNaN(trimmed) ? Number(trimmed) : trimmed;
            })
        );
    }
};