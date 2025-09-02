import { useState, useMemo, memo } from "react";
import { Grid, Box, Button, Typography } from "@mui/material";
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
import background from "../../assets/backgrounds/background3.jpg";


const initialMapCenter = [-45.86168350577915, -67.5188749673741]; // Comodoro Rivadavia, Argentina
const initialZoom = 13;


const View = () => {
    const [featureCollection, setFeatureCollection] = useState({features:[]});
    const [elevationData, setElevationData] = useState([]);

    const toast = useToast();

    const onInputLoaded = (data, format) => {
        switch(format){
            case "geojson":
                const featureCollection = JSON.parse(data);
                toast(`GeoJSON cargado. Total de features: ${featureCollection.features.length}`, "success");
                setFeatureCollection(featureCollection);
                break;
            case "csv":
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

    const buttonDisabled = memoizedFeatureCollection.features.length === 0 || memoizedElevationData.length === 0;

    return(
        <MainView background={background}>
            <Grid container spacing={2} direction="row" sx={{height:"75vh"}}>
                <Grid size={4}>
                    <Grid container spacing={1} direction="column">
                        <Grid size={12}>
                            <Box sx={{height: "100%", width: "100%"}}>
                                <Typography variant="h5" sx={{m:2}}>Carga de datos</Typography>
                                <DropZone onDrop={(data, format) => onInputLoaded(data, format)} />
                            </Box>
                        </Grid>
                        <Grid size={12}>
                            <Box>
                                <Typography variant="h5" sx={{m:2}}>Acciones</Typography>
                                <Button 
                                    variant={"contained"} 
                                    sx={{marginRight:1, marginBottom:1}}
                                    disabled={buttonDisabled}>
                                    Computar líneas de vista
                                </Button>
                                <Button 
                                    variant={"contained"} 
                                    sx={{marginRight:1, marginBottom:1}}
                                    disabled={buttonDisabled}>
                                    Reposicionar Gateways
                                </Button>
                            </Box>  
                        </Grid>
                    </Grid>
                </Grid>
                <Grid size={8}>
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