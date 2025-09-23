import { useState } from "react";
import useComputations from "../useComputations";
import useToast from "../useToast";
import usePreloader from "../usePreloader";
import { useFilesContext } from "../../context/Files";


const useAnalysis = () => {
    const toast = useToast();
    const preloader = usePreloader();
    
    const [losResult, setLosResult] = useState(null);
    
    const { computeLOS, evalNetwork, runSolver } = useComputations();

    // files is used to get the uploaded files and their IDs
    // setFiles is used to set the result of computations in the features content
    const { files, setFiles } = useFilesContext();

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

    const solverAction = async solver => {
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
        const result = await solver(params);
        if(!result.error){
            const nextFiles = {
                ...files,
                features: {
                    id: files.features.id,
                    content: result
                }
            }
            setFiles(nextFiles);
        }else{
            toast(result.error, "error");
        }
        preloader(false);
    };

    const evalNetworkAction = async () => {
        await solverAction(evalNetwork);
    };

    const runSolverAction = async () => {
        await solverAction(runSolver);
    };

    const resetLOS = () => setLosResult(null);

    const resetNetworkConnection = () => {
        const nextFiles = {
            ...files,
            features: {
                id: files.features.id,
                content: null
            }
        };
        setFiles(nextFiles);
    }

    return {
        losResult,
        computeLOSAction,
        evalNetworkAction,
        runSolverAction,
        resetLOS,
        resetNetworkConnection
    };
};

export default useAnalysis;