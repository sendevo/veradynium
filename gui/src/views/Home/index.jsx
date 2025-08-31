import { useState, useMemo, memo } from "react";
import { Grid, Box } from "@mui/material";
import { 
    csvToArray, 
    chunkedMax,
    chunkedMin,
    normalizeElevation } from "../../model/utils";
import MainView from "../../components/MainView";
import Map from "../../components/Map";
import DropZone from "../../components/DropZone";
import useToast from "../../hooks/useToast";
import background from "../../assets/backgrounds/background3.jpg";

const mapCenter = [-45.86168350577915, -67.5188749673741]; // Comodoro Rivadavia, Argentina

const View = () => {
    const [featureCollection, setFeatureCollection] = useState({features:[]});
    const [elevationData, setElevationData] = useState([]);

    const toast = useToast();

    const onInputLoaded = (data, format) => {
        switch(format){
            case "geojson":
                const featureCollection = JSON.parse(data);
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

    return(
        <MainView background={background}>
            <Grid container spacing={2} direction="row">
                <Grid size={4}>
                    <Box sx={{height: "100%", width: "100%", display: "flex", alignItems: "center", justifyContent: "center"}}>
                        <DropZone onDrop={(data, format) => onInputLoaded(data, format)} />
                    </Box>
                </Grid>
                <Grid size={8}>
                    <Map 
                        mapCenter={mapCenter}
                        featureCollection={memoizedFeatureCollection}
                        elevationData={memoizedElevationData}/>
                </Grid>
            </Grid>
        </MainView>
    )
};

export default View;