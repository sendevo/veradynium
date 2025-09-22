import { useState, useEffect } from "react";
import { Grid } from "@mui/material";
import MainView from "../../components/MainView";
import Map from "../../components/Map";
import Controls from "../../components/Controls";
import LOSResultsModal from "./losResultsModal";
import NetworkResultsModal from "./networkResultsModal";
import useAnalysis from "../../hooks/useAnalysis";
import background from "../../assets/backgrounds/background3.jpg";


const initialMapCenter = [-45.86168350577915, -67.5188749673741]; // Comodoro Rivadavia, Argentina
const initialZoom = 13;


const View = () => {
    const [points, setPoints] = useState([]); // Points for LOS calculation
    // Modals state
    const [losResultModalOpen, setLosResultModalOpen] = useState(false);
    const [networkResultModalOpen, setNetworkResultModalOpen] = useState(false);

    const [elevationData, setElevationData] = useState(null);
    const [features, setFeatures] = useState(null);

    const {
        losResult,
        networkResult,
        evalNetworkAction,
        computeLOSAction,
        runSolverAction,
        resetLOS,
        resetNetworkConnection,
        resetSolverResults
    } = useAnalysis();

    // When network connections are available, use them as features, else use uploaded features
    const featureCollection = networkResult || features;

    useEffect(() => {
        if(losResult)
            setLosResultModalOpen(true);
    }, [losResult]);

    useEffect(() => {
        if(networkResult)
            setNetworkResultModalOpen(true);
    }, [networkResult]);

    const handleAddElevation = (data, resetConnections = false) => {
        setElevationData(data);
        if(resetConnections){
            resetNetworkConnection();
            resetSolverResults();
        }
    };

    const handleNewPoints = newPoints => {
        resetLOS();
        setPoints(newPoints);
    };

    const handleComputeLOS = async () => {
        computeLOSAction(points);
    };

    const handleRemoveElevation = () => {
        resetLOS();
        resetNetworkConnection();
        setPoints([]);
    };

    const handleRemoveFeatures = () => {
        resetNetworkConnection();
        resetSolverResults(); 
    };

    const handleResetPoints = () => { 
        resetLOS(); 
        setPoints([]); 
    };

    return(
        <MainView background={background}>
            <Grid container spacing={2} direction="row" sx={{height:"75vh"}}>
                <Grid size={3}>
                    <Grid container direction={"column"} spacing={2} sx={{height:"100%"}}>
                        <Grid size={"grow"}>
                            <Controls 
                                onAddElevation={handleAddElevation}
                                onAddFeatures={setFeatures}
                                onRemoveElevation={handleRemoveElevation}
                                onRemoveFeatures={handleRemoveFeatures}
                                handleResetPoints={handleResetPoints}
                                handleComputeLOS={handleComputeLOS}
                                evalNetworkAction={evalNetworkAction}
                                runSolverAction={runSolverAction}
                                points={points}/>
                        </Grid>

                        <LOSResultsModal result={losResult} open={losResultModalOpen} onClose={() => setLosResultModalOpen(false)}/>

                        <NetworkResultsModal result={networkResult} open={networkResultModalOpen} onClose={() => setNetworkResultModalOpen(false)}/>
                        
                    </Grid>
                </Grid>

                <Grid size={9}>
                    <Map 
                        mapCenter={initialMapCenter}
                        initialZoom={initialZoom}
                        featureCollection={featureCollection || { features: [] }}
                        elevationData={elevationData || []}
                        points={points}
                        setPoints={handleNewPoints}/>
                </Grid>
            </Grid>
        </MainView>
    )
};

export default View;