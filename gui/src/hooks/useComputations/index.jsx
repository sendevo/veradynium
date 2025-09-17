import { useCallback } from "react";
import { api } from '../../model/constants';
import { fetchWithTimeout } from "../../model/utils";


const useComputations = () => {

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
                return { error: "Ocurrió un error al calcular LOS" };
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
                return { error: "Ocurrió un error al procesar la solicitud" };
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
                return { error: "Ocurrió un error al procesar la solicitud" };
            }
        }

        const data = await res.json();
        /* output format:
        {
            
        }
        */
        return data;
    }, []);

    return { computeLOS, evalNetwork, runSolver  };
};

export default useComputations;