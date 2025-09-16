import { useState } from "react";
import useComputations from "../useComputations";
import useToast from "../useToast";
import usePreloader from "../usePreloader";
import { useFilesContext } from "../../context/Files";


const useAnalysis = () => {
    const toast = useToast();
    const preloader = usePreloader();
    
    const [losResult, setLosResult] = useState(null);
    const [solverResult, setSolverResult] = useState(null);

    const { computeLOS, runSolver } = useComputations();

    const { files } = useFilesContext();

    const computeLOSAction = async (points) => {
        if (points.length < 2) {
            toast("Coordenadas de prueba no definidas", "error");
            return;
        }
        if (!files.elevation_map.id) {
            toast("El mapa de elevación no está disponible", "error");
            return;
        }

        const params = {
            em_file_id: files.elevation_map.id,
            p1: points[0],
            p2: points[1],
        };

        preloader(true);
        console.log("Computing LOS with params:", params);
        const result = await computeLOS(params);
        if(!result.error){
            console.log("LOS result:", result);
            setLosResult(result);
        }else{
            toast(result.error, "error");
        }
        preloader(false);
    };

    const runSolverAction = async () => {
        if (!files.elevation_map.id) {
            toast("El mapa de elevación no está disponible", "error");
            return;
        }
        if (!files.features.id) {
            toast("El archivo de geometrías no está disponible", "error");
            return;
        }

        const params = {
            em_file_id: files.elevation_map.id,
            features_file_id: files.features.id
        };

        preloader(true);
        const result = await runSolver(params);
        if(!result.error){
            console.log(result);
            setSolverResult(result);
        }else{
            toast(result.error, "error");
        }
        preloader(false);
    };

    const resetLOS = () => setLosResult(null);

    const resetResults = () => setSolverResult(null);

    return {
        losResult,
        solverResult,
        computeLOSAction,
        runSolverAction,
        resetLOS,
        resetResults
    };
};

export default useAnalysis;