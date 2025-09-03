import { useState, useMemo } from "react";
import { Grid, Box, Button } from "@mui/material";
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

    const toast = useToast();

    //// TEST
     const { fileIds } = useFileIdsContext();
    const { los } = useComputations();
    const handleComputeLOS = async () => {
        const params = {
            em_file_id: fileIds.em_file,
            p1: {
                lat: -45.825412,
                lon: -67.45874,
                height_m: 2.0
            },
            p2: {
                lat: -45.82915,
                lon: -67.45874,
                height_m: 2.5
            }
        };
        if (!params.em_file_id) {
            toast("Por favor, cargue un archivo de mapa de elevación (CSV) antes de calcular la línea de vista.", "error");
            return;
        }
        console.log("Computing LOS with params:", params);
        const result = await los(params);
        console.log("LOS result:", result);
    }
    /// TEST

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
                    <Box sx={{width: "100%", height: "100%"}}>
                        <DropZone 
                            onDrop={(data, extension) => onInputLoaded(data, extension)} 
                            onError={message => toast(message, "error")}/>

                        <Button 
                            onClick={handleComputeLOS}
                            variant="contained"
                            sx={{m:2}}>
                            Calcular LOS
                        </Button>
                    </Box>
                </Grid>
                <Grid size={9}>
                    <Map 
                        mapCenter={initialMapCenter}
                        initialZoom={initialZoom}
                        featureCollection={memoizedFeatureCollection}
                        elevationData={memoizedElevationData}/>
                </Grid>
            </Grid>
        </MainView>
    )
};

export default View;