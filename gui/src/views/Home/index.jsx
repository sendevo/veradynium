import { useState } from "react";
import { Grid, Typography } from "@mui/material";
import MainView from "../../components/MainView";
import Map from "../../components/Map";
import DropZone from "../../components/DropZone";
import ResultsModal from "./resultsModal";
import MenuButtons from "./menuButtons";
import useToast from "../../hooks/useToast";
import { useFilesContext } from "../../context/Files";
import useAnalysis from "../../hooks/useAnalysis";
import background from "../../assets/backgrounds/background3.jpg";


const initialMapCenter = [-45.86168350577915, -67.5188749673741]; // Comodoro Rivadavia, Argentina
const initialZoom = 13;


const View = () => {
    const [points, setPoints] = useState([]); // Points for LOS calculation

    const toast = useToast();

    const {
        files,
        uploadFile,
        removeFile
    } = useFilesContext();

    const {
        losResult,
        evalNetworkAction,
        computeLOSAction,
        runSolverAction,
        resetLOS,
        resetSolverResults
    } = useAnalysis();

    const elevationData = files.elevation_map.content || [];
    const featureCollection = files.features.content || { features: [] };

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
        setPoints([]);
    };

    const handleRemoveFeatures = () => {
        removeFile(files.features.id, ".geojson");
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
                                onDrop={uploadFile} 
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

                        {losResult && <ResultsModal result={losResult}/>}

                        
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