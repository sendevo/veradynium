import { useCallback } from "react";
import { api } from '../../model/constants';


const useComputations = () => {

    const computeLOS = useCallback( async params => { 
        // params format: {em_file_id, p1: {lat, lng, height_m}, p2: {lat, lng, height_m}}

        const res = await fetch(api("/api/los"), {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(params)
        });

        if (!res.ok) {
            //const text = await res.text();
            //throw new Error(`Error response: ${res.status} - ${text}`);
            return null;
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

    const runSolver = useCallback( async params => {
        // params format: {em_file_id, features_file_id}
        
        const res = await fetch(api("/api/solve"), {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(params)
        });

        if (!res.ok) {
            //const text = await res.text();
            //throw new Error(`Error response: ${res.status} - ${text}`);
            return null;
        }

        const data = await res.json();
        /* output format:
        {
            
        }
        */
        return data;
    }, []);

    return { computeLOS, runSolver  };
};

export default useComputations;