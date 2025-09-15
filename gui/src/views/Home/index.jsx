import { useState, useMemo } from "react";
import { Grid, Button, Typography } from "@mui/material";
import MainView from "../../components/MainView";
import Map from "../../components/Map";
import DropZone from "../../components/DropZone";
import ResultsModal from "./resultsModal";
import useToast from "../../hooks/useToast";
import usePreloader from "../../hooks/usePreloader";
import useFiles from "../../hooks/useFiles";
import useAnalysis from "../../hooks/useAnalysis";
import background from "../../assets/backgrounds/background3.jpg";


const initialMapCenter = [-45.86168350577915, -67.5188749673741]; // Comodoro Rivadavia, Argentina
const initialZoom = 13;


const View = () => {
    const [points, setPoints] = useState([]); // Points for LOS calculation

    const toast = useToast();
    const preloader = usePreloader();

    const {
        files,
        uploadFile,
        removeFile
    } = useFiles(toast, preloader);

    const {
        losResult,
        computeLOSAction,
        runSolverAction,
        resetLOS,
        resetResults
    } = useAnalysis(toast, preloader, files);

    const elevationData = files.elevation_map.content || [];
    const featureCollection = files.features.content || { features: [] };

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
        removeFile(files.elevation_map.id, ".csv");
        resetLOS();
        setPoints([]);
    };

    const handleRemoveFeatures = () => {
        removeFile(files.features.id, ".geojson");
        resetResults();
    };

    const memoizedElevationData = useMemo(() => elevationData, [elevationData]);
    const memoizedFeatureCollection = useMemo(() => featureCollection, [featureCollection]);

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

                        {(hasElevation > 0 || hasFeatures > 0) &&
                            <Grid>
                                <Typography sx={{fontWeight:"bold"}}>Sincronización de datos</Typography>
                                {hasElevation ? 
                                    <Typography sx={{fontSize: 12}}>Mapa de elevación cargado</Typography>
                                    :
                                    <Typography sx={{fontSize: 12}}>Mapa de elevación en modo local</Typography>
                                }
                                {hasFeatures ?
                                    <Typography sx={{fontSize: 12}}>Geometrías cargadas</Typography>
                                    :
                                    <Typography sx={{fontSize: 12}}>Geometrías en modo local</Typography>
                                }
                            </Grid>
                        }

                        {losResult && <ResultsModal result={losResult}/>}

                        {hasFeatures > 0 &&
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

                        {hasElevation > 0 &&
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

                        {points.length === 2 && hasElevation &&
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

                        {hasFeatures && hasElevation &&
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