export const removeSlash = path => path.startsWith('/') ? path.slice(1) : path;

export const fetchWithTimeout = async (url, options = {}, timeout = 5000) => {
    const controller = new AbortController();
    const id = setTimeout(() => controller.abort(), timeout);
    try {
        const response = await fetch(url, {
            ...options,
            signal: controller.signal,
        });
        return response;
    } catch (err) {
        if (err.name === "AbortError") {
            throw new Error("Request timed out");
        }
        throw err;
    } finally {
        clearTimeout(id);
    }
};

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
                return ".geojson";
            else
                return ".json";
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
                return ".csv";
            }
        }
    }

    return "unknown";
}

export const csvToArray = (csvString, options) => {

    const defaultOptions = {
        withHeaders: true,
        delimiter: ",",
        maxRows: -1
    };

    const {
        withHeaders,
        delimiter,
        maxRows
    } = {
        ...defaultOptions,
        ...options
    };

    const lines = csvString.trim().split(/\r?\n/);

    if (lines.length === 0 || maxRows === 0) return [];

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
        /*
        // Return 2D array (matrix)
        let result = lines.slice(1); // Skip headers
        if (maxRows > 0 && lines.length > maxRows){
            const step = Math.floor(lines.length / maxRows);
            result = [...result.filter((_, index) => index % step === 0)]; // Subsample
        }        
        return result.map(line =>
            line.split(delimiter).map(v => {
                const trimmed = v.trim();
                return trimmed !== "" && !isNaN(trimmed) ? Number(trimmed) : trimmed;
            })
        );
        */
        let result = lines.slice(1); // Skip headers
        if (maxRows > 0 && result.length > maxRows) {
            // Fisher-Yates shuffle algorithm for random sampling
            const shuffled = [...result];
            for (let i = shuffled.length - 1; i > 0; i--) {
                const j = Math.floor(Math.random() * (i + 1));
                [shuffled[i], shuffled[j]] = [shuffled[j], shuffled[i]];
            }
            result = shuffled.slice(0, maxRows);
        }
        return result.map(line =>
            line.split(delimiter).map(v => {
                const trimmed = v.trim();
                return trimmed !== "" && !isNaN(trimmed) ? Number(trimmed) : trimmed;
            })
        );

    }
};

export const chunkedMax = (array, chunkSize = 10000) => {
    let max = -Infinity;
    for (let i = 0; i < array.length; i += chunkSize) {
        const chunk = array.slice(i, i + chunkSize);
        const chunkMax = Math.max(...chunk);
        if (chunkMax > max) max = chunkMax;
    }
    return max;
}

export const chunkedMin = (array, chunkSize = 10000) => {
    let min = Infinity;
    for (let i = 0; i < array.length; i += chunkSize) {
        const chunk = array.slice(i, i + chunkSize);
        const chunkMin = Math.min(...chunk);
        if (chunkMin < min) min = chunkMin;
    }
    return min;
}

export const normalizeElevation = (data, min, max) => {
    // Data is expected to be an array of [lat, lng, elevation]
    // Returns the same structure with normalized elevation values (between 0 and 1)

    if (data.length === 0) return [];

    // Normalize elevations to 0-1 range for better color distribution
    return data.map(point => [
        point[0], // lat
        point[1], // lng
        (point[2] - min) / (max - min) // normalized elevation
    ]);
};