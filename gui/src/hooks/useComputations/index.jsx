import { useCallback } from "react";
import { useTranslation } from "react-i18next";
import { api } from '../../model/constants';
import { fetchWithTimeout } from "../../model/utils";


const useComputations = () => {

    const { t } = useTranslation("error");

    const computeLOS = useCallback( async params => { 
        // params format: {em_file_id, p1: {lat, lng, height_m}, p2: {lat, lng, height_m}}

        const res = await fetchWithTimeout(api("/api/los"), {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(params)
        });

        if (!res.ok) {
            //const text = await res.text();
            //throw new Error(`Error response: ${res.status} - ${text}`);
            const text = await res.text();
            try{
                const error = JSON.parse(text);
                const errorMessage = error.error || text;
                console.error("Error response:", res.status, errorMessage);
                return { error: errorMessage };
            }catch{
                console.error("Error response:", res.status, text);
                return { error: t("los_error") };
            }
        }

        const data = await res.json();
        /* output format:
        { 
            line_of_sight: true | false, 
            distance_m: number, 
            point1: {lat, lng, height_m}, 
            point2: {lat, lng, height_m} 
        }
        */
        return data;
    }, []);

    const evalNetwork = useCallback( async params => {
        // params format: {em_file_id, features_file_id}
        
        console.log("Evaluating network with params:", params);

        const res = await fetchWithTimeout(api("/api/eval"), {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(params)
        });

        if (!res.ok) {
            const text = await res.text();
            try{
                const error = JSON.parse(text);
                const errorMessage = error.error || text;
                console.error("Error response:", res.status, errorMessage);
                return { error: errorMessage };
            }catch{
                console.error("Error response:", res.status, text);
                return { error: t("request_error") };
            }
        }

        const data = await res.json();
        /* output format: GeoJSON FeatureCollection */
        return data;
    }, []);

    const runSolver = useCallback( async params => {
        // params format: {em_file_id, features_file_id}
        
        const res = await fetchWithTimeout(api("/api/solve"), {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(params),
            timeout: 120000 // 2 minutes
        });

        if (!res.ok) {
            const text = await res.text();
            try{
                const error = JSON.parse(text);
                const errorMessage = error.error || text;
                console.error("Error response:", res.status, errorMessage);
                return { error: errorMessage };
            }catch{
                console.error("Error response:", res.status, text);
                return { error: t("request_error") };
            }
        }

        const data = await res.json();
        /* output format:
        {
            bbox: [minLng, minLat, maxLng, maxLat],
            features: GeoJSON FeatureCollection,
            properties: {
                connected_end_devices: number,
                disconnected_end_devices: number,
                distance_histogram: [],
                distance_histogram_bin_size: number,
                elevation_grid: {
                    altitude_range: [minAltitude, maxAltitude],
                    bounding_box: {
                        bottom_left: [lng, lat],
                        upper_right: [lng, lat]
                    }
                },
                max_connection_distance: number,
                network_bbox: {
                    bottom_left: [lng, lat],
                    upper_right: [lng, lat]
                },
                num_end_devices: number,
                num_gateways: number,
                total_distance: number
            }
        }
        */
        return data;
    }, []);

    return { computeLOS, evalNetwork, runSolver  };
};

export default useComputations;