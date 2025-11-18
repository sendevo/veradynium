import { useState } from "react";
import { useTranslation } from "react-i18next";
import useComputations from "../useComputations";
import useToast from "../useToast";
import usePreloader from "../usePreloader";
import { useModelContext } from "../../context/Model";


const useAnalysis = () => {
    const toast = useToast();
    const preloader = usePreloader();
    
    const { t } = useTranslation("use_analysis");

    const [losResult, setLosResult] = useState(null);
    
    const { computeLOS, evalNetwork, runSolver } = useComputations();

    // model is used to get the uploaded model and their IDs
    // setModel is used to set the result of computations in the features content
    const { model, setModel } = useModelContext();

    const computeLOSAction = async points => {
        if (points.length < 2) {
            toast(t("los_coordinates_undefined"), "error");
            return;
        }
        if (!model.elevation_map.id) {
            toast(t("elevation_map_unavailable"), "error");
            return;
        }

        const params = {
            em_file_id: model.elevation_map.id,
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

    const solverAction = async (solver, args={}) => {
        if (!model.elevation_map.id) {
            toast(t("elevation_map_unavailable"), "error");
            return;
        }
        if (!model.features.id) {
            toast(t("geometries_file_unavailable"), "error");
            return;
        }

        const params = {
            em_file_id: model.elevation_map.id,
            features_file_id: model.features.id,
            ...args
        };

        preloader(true);
        console.log("Running solver with params:", params);
        const result = await solver(params);
        if(!result.error){
            const nextModel = {
                ...model,
                features: {
                    id: model.features.id,
                    content: result
                }
            }
            setModel(nextModel);
        }else{
            toast(result.error, "error");
        }
        preloader(false);
    };

    const evalNetworkAction = async () => {
        await solverAction(evalNetwork);
    };

    const runSolverAction = async method => {
        await solverAction(runSolver, {method});
    };

    const resetLOS = () => setLosResult(null);

    const resetNetworkConnection = () => {
        const nextModel = {
            ...model,
            features: {
                id: model.features.id,
                content: null
            }
        };
        setModel(nextModel);
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