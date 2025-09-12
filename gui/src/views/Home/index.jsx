import { useState, useMemo } from "react";
import { Grid, Button, Typography } from "@mui/material";
import MainView from "../../components/MainView";
import Map from "../../components/Map";
import DropZone from "../../components/DropZone";
import ResultsModal from "./resultsModal";
import useToast from "../../hooks/useToast";
import usePreloader from "../../hooks/usePreloader";
import useFileLoader from "../../hooks/useFileLoader";
import useAnalysis from "../../hooks/useAnalysis";
import background from "../../assets/backgrounds/background3.jpg";


const initialMapCenter = [-45.86168350577915, -67.5188749673741]; // Comodoro Rivadavia, Argentina
const initialZoom = 13;


const View = () => {
    const [points, setPoints] = useState([]); // Points for LOS calculation

    const toast = useToast();
    const preloader = usePreloader();

    const {
        losResult,
        computeLOSAction,
        runSolverAction,
        resetLOS,
        resetResults
    } = useAnalysis(toast, preloader);

    const {
        fileIds,
        featureCollection,
        elevationData,
        loadFile,
        removeElements,
    } = useFileLoader(toast);

    const handleNewPoints = newPoints => {
        resetLOS();
        setPoints(newPoints);
    };

    const handleComputeLOS = async () => {
        computeLOSAction(points);
    };

    const handleRunSolver = async () => {
        runSolverAction();
    };

    const handleRemoveElevation = () => {
        removeElements("elevation");
        resetLOS();
        setPoints([]);
    };

    const handleRemoveFeatures = () => {
        removeElements("features");
        resetResults();
    };

    const memoizedElevationData = useMemo(() => elevationData, [elevationData]);
    const memoizedFeatureCollection = useMemo(() => featureCollection, [featureCollection]);

    return(
        <MainView background={background}>
            <Grid container spacing={2} direction="row" sx={{height:"75vh"}}>
                <Grid size={3}>
                    <Grid container direction={"column"} spacing={2} sx={{height:"100%"}}>
                        <Grid size={"grow"}>
                            <DropZone 
                                onDrop={(data, extension) => loadFile(data, extension)} 
                                onError={message => toast(message, "error")}/>
                        </Grid>

                        {(featureCollection.features.length > 0 || elevationData.length > 0) &&
                            <Grid>
                                <Typography sx={{fontWeight:"bold"}}>Sincronización de datos</Typography>
                                {fileIds.em_file ? 
                                    <Typography sx={{fontSize: 12}}>Mapa de elevación cargado</Typography>
                                    :
                                    <Typography sx={{fontSize: 12}}>Mapa de elevación en modo local</Typography>
                                }
                                {fileIds.features_file ?
                                    <Typography sx={{fontSize: 12}}>Geometrías cargadas</Typography>
                                    :
                                    <Typography sx={{fontSize: 12}}>Geometrías en modo local</Typography>
                                }
                            </Grid>
                        }

                        {losResult && <ResultsModal {...losResult} />/*Not a modal for now*/}

                        {featureCollection.features.length > 0 &&
                            <Grid>
                                <Button 
                                    fullWidth
                                    color="secondary"
                                    onClick={handleRemoveFeatures}
                                    variant="contained">
                                        Quitar geometrías
                                </Button>
                            </Grid>
                        }

                        {elevationData.length > 0 &&
                            <Grid>
                                <Button 
                                    fullWidth
                                    color="secondary"
                                    onClick={handleRemoveElevation}
                                    variant="contained">
                                        Quitar altimetría
                                </Button>
                            </Grid>
                        }

                        {points.length === 2 && elevationData.length > 0 &&
                            <>
                                <Grid>
                                    <Button 
                                        fullWidth
                                        color="secondary"
                                        onClick={() => { resetLOS(); setPoints([]); }}
                                        variant="contained">
                                            Restablecer puntos
                                    </Button>
                                </Grid>
                                <Grid>
                                    <Button 
                                        fullWidth
                                        onClick={handleComputeLOS}
                                        variant="contained">
                                            Calcular LOS
                                    </Button>
                                </Grid>
                            </>
                        }

                        {featureCollection.features.length > 0 && elevationData.length > 0 &&
                            <Grid>
                                <Button 
                                    fullWidth
                                    onClick={handleRunSolver}
                                    variant="contained">
                                        Ejecutar solver
                                </Button>
                            </Grid>
                        }
                    </Grid>
                </Grid>
                <Grid size={9}>
                    <Map 
                        mapCenter={initialMapCenter}
                        initialZoom={initialZoom}
                        featureCollection={memoizedFeatureCollection}
                        elevationData={memoizedElevationData}
                        points={points}
                        setPoints={handleNewPoints}/>
                </Grid>
            </Grid>
        </MainView>
    )
};

export default View;