import { useState } from "react";
import { useFileIdsContext } from "../../context/FileIds";
import useComputations from "../useComputations";


const useAnalysis = (toast, preloader) => {
    const [losResult, setLosResult] = useState(null);
    const [solverResult, setSolverResult] = useState(null);

    const { fileIds } = useFileIdsContext();
    const { computeLOS, runSolver } = useComputations();

    const computeLOSAction = async (points) => {
        if (points.length < 2) {
            toast("Coordenadas de prueba no definidas", "error");
            return;
        }
        if (!fileIds.em_file) {
            toast("El mapa de elevación no está disponible", "error");
            return;
        }

        const params = {
            em_file_id: fileIds.em_file,
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
        if (!fileIds.em_file) {
            toast("El mapa de elevación no está disponible", "error");
            return;
        }
        if (!fileIds.features_file) {
            toast("El archivo de geometrías no está disponible", "error");
            return;
        }

        const params = {
            em_file_id: fileIds.em_file,
            features_file_id: fileIds.features_file,
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