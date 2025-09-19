import { useState, useEffect } from "react";
import { Grid, Typography } from "@mui/material";
import MainView from "../../components/MainView";
import Map from "../../components/Map";
import DropZone from "../../components/DropZone";
import LOSResultsModal from "./losResultsModal";
import NetworkResultsModal from "./networkResultsModal";
import MenuButtons from "./menuButtons";
import useToast from "../../hooks/useToast";
import { useFilesContext } from "../../context/Files";
import useAnalysis from "../../hooks/useAnalysis";
import background from "../../assets/backgrounds/background3.jpg";


const initialMapCenter = [-45.86168350577915, -67.5188749673741]; // Comodoro Rivadavia, Argentina
const initialZoom = 13;


const View = () => {
    const [points, setPoints] = useState([]); // Points for LOS calculation
    // Modals state
    const [losResultModalOpen, setLosResultModalOpen] = useState(false);
    const [networkResultModalOpen, setNetworkResultModalOpen] = useState(false);

    const toast = useToast();

    const {
        files,
        uploadFile,
        removeFile
    } = useFilesContext();

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

    useEffect(() => {
        if(losResult)
            setLosResultModalOpen(true);
    }, [losResult]);

    useEffect(() => {
        if(networkResult)
            setNetworkResultModalOpen(true);
    }, [networkResult]);

    const elevationData = files.elevation_map.content || [];
    const featureCollection = networkResult || files.features.content || { features: [] };
    
    const handleUploadFile = file => {
        const extension = "." + file.name.split(".").pop().toLowerCase();
        if(extension === ".json")
            resetNetworkConnection();
        uploadFile(file);
    }

    const handleNewPoints = newPoints => {
        resetLOS();
        setPoints(newPoints);
    };

    const handleComputeLOS = async () => {
        computeLOSAction(points);
    };

    const handleRemoveElevation = () => {
        removeFile(files.elevation_map.id, ".csv");
        resetLOS();
        resetNetworkConnection();
        setPoints([]);
    };

    const handleRemoveFeatures = () => {
        removeFile(files.features.id, ".geojson");
        resetNetworkConnection();
        resetSolverResults(); 
    };

    const handleResetPoints = () => { 
        resetLOS(); 
        setPoints([]); 
    };

    const hasElevation = elevationData && elevationData.length > 0;
    const hasFeatures = featureCollection && featureCollection.features && featureCollection.features.length > 0;

    return(
        <MainView background={background}>
            <Grid container spacing={2} direction="row" sx={{height:"75vh"}}>
                <Grid size={3}>
                    <Grid container direction={"column"} spacing={2} sx={{height:"100%"}}>
                        <Grid size={"grow"}>
                            <DropZone 
                                onDrop={handleUploadFile} 
                                onError={message => toast(message, "error")}/>
                        </Grid>

                        {(hasElevation || hasFeatures) && 
                            <Grid>
                                <Typography>Estado de los archivos:</Typography>
                            
                                {hasElevation && 
                                    <>
                                        {files.elevation_map.id ? 
                                            <Typography sx={{fontSize: 12}}>Mapa de elevación cargado</Typography>
                                            :
                                            <Typography sx={{fontSize: 12}}>Mapa de elevación en modo local</Typography>
                                        }
                                    </>
                                }
                                {hasFeatures &&  
                                    <>
                                        {files.features.id ?
                                            <Typography sx={{fontSize: 12}}>Geometrías cargadas</Typography>
                                            :
                                            <Typography sx={{fontSize: 12}}>Geometrías en modo local</Typography>
                                        }
                                    </>
                                }
                            </Grid>
                        }

                        <MenuButtons
                            hasFeatures={hasFeatures}
                            hasElevation={hasElevation}
                            points={points}
                            handleRemoveFeatures={handleRemoveFeatures}
                            handleRemoveElevation={handleRemoveElevation}
                            handleResetPoints={handleResetPoints}
                            handleEvalNetwork={evalNetworkAction}
                            handleComputeLOS={handleComputeLOS}
                            handleRunSolver={runSolverAction}/>

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