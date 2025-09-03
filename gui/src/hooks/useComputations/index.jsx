import { useCallback } from "react";
import { api } from '../../model/constants';


const useComputations = () => {

    const los = useCallback( async params => { 
        // params format: {em_file_id, p1: {lat, lon, height_m}, p2: {lat, lon, height_m}}

        const res = await fetch(api("/api/los"), {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(params)
        });

        if (!res.ok) {
            const text = await res.text();
            throw new Error(`Error response: ${res.status} - ${text}`);
        }

        const data = await res.json();
        /* output format:
        { 
            line_of_sight: true | false, 
            distance_m: number, 
            point1: {lat, lon, height_m}, 
            point2: {lat, lon, height_m} 
        }
        */
        return data;
    }, []);

    const solve = useCallback( async params => {
        console.log("Solve params:", params);
    }, []);

    return { los, solve  };
};

export default useComputations;