import { useState, useMemo } from "react";
import { Grid, Button, Box, Typography } from "@mui/material";
import { 
    csvToArray, 
    chunkedMax,
    chunkedMin,
    normalizeElevation 
} from "../../model/utils";
import MainView from "../../components/MainView";
import Map from "../../components/Map";
import DropZone from "../../components/DropZone";
import useToast from "../../hooks/useToast";
import { useFileIdsContext } from "../../context/FileIds";
import useComputations from "../../hooks/useComputations";
import background from "../../assets/backgrounds/background3.jpg";


const initialMapCenter = [-45.86168350577915, -67.5188749673741]; // Comodoro Rivadavia, Argentina
const initialZoom = 13;


const View = () => {
    const [featureCollection, setFeatureCollection] = useState({features:[]});
    const [elevationData, setElevationData] = useState([]);
    const [points, setPoints] = useState([]); // Points for LOS calculation
    const [losResult, setLosResult] = useState(null);

    const { fileIds } = useFileIdsContext();
    const { computeLOS } = useComputations();
    const toast = useToast();

    const handleNewPoints = newPoints => {
        setLosResult(null);
        setPoints(newPoints);
    };

    const handleComputeLOS = async () => {
        
        if(points.length < 2){
            toast("Coordenadas de prueba no definidas", "error");
            return;
        }

        if (!fileIds.em_file) {
            toast("Mapa de elevación de terreno no definido", "error");
            return;
        }

        const params = {
            em_file_id: fileIds.em_file,
            p1: points[0],
            p2: points[1]
        };

        console.log("Computing LOS with params:", params);
        const result = await computeLOS(params);
        if(result)
            setLosResult(result);
        else
            toast("Ocurrió un error durante el cálculo de LOS", "error");
    }

    const onInputLoaded = (data, extension) => {
        switch(extension){
            case "geojson": // Feature collection -> Draw features
                const featureCollection = JSON.parse(data);
                toast(`GeoJSON cargado. Total de features: ${featureCollection.features.length}`, "success");
                setFeatureCollection(featureCollection);
                break;
            case "csv": // Elevation data -> Draw heatmap
                const options = {
                    withHeaders: false,
                    delimiter: ",",
                    maxRows: -1 // All data
                };
                const arr = csvToArray(data, options);
                const alts = arr.map(p => p[2]);
                const normalizedElevation = normalizeElevation(arr, chunkedMin(alts), chunkedMax(alts));
                toast(`Datos de elevación cargados. Total de puntos: ${arr.length}`, "success");
                setElevationData(normalizedElevation);
                break;
            case "json":
            case "unknown":
            default:
                toast("Formato no soportado", "error");
        }
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
                                onDrop={(data, extension) => onInputLoaded(data, extension)} 
                                onError={message => toast(message, "error")}
                                />
                        </Grid>
                        <Grid>
                            {losResult && 
                                <Box>
                                    <Typography sx={{fontWeight:"bold"}}>Punto 1</Typography>
                                    <Typography>lat: {losResult.point1.lat}</Typography>
                                    <Typography>lon: {losResult.point1.lng}</Typography>
                                    <Typography>elev: {losResult.point1.height_m} m</Typography>

                                    <Typography sx={{fontWeight:"bold", mt:1}}>Punto 2</Typography>
                                    <Typography>lat: {losResult.point2.lat}</Typography>
                                    <Typography>lon: {losResult.point2.lng}</Typography>
                                    <Typography>elev: {losResult.point2.height_m} m</Typography>
                                    
                                    <Typography sx={{mt:1}}><b>Distancia:</b> {losResult.distance_m} m</Typography>

                                    <Typography sx={{mt:1}}><b>Línea de vista:</b> {losResult.line_of_sight ? "Si" : "No"}</Typography>
                                </Box>
                            }
                            {points.length === 2 &&
                                <Box sx={{mt:2}}>
                                    <Button 
                                        fullWidth
                                        onClick={handleComputeLOS}
                                        variant="contained">
                                            Calcular LOS
                                    </Button>
                                </Box>
                            }
                        </Grid>
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