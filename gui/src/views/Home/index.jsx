import { useState } from "react";
import { Grid, Box } from "@mui/material";
import { isValidGeoJSON } from "../../model/utils";
import MainView from "../../components/MainView";
import Map from "../../components/Map";
import DropZone from "../../components/DropZone";
import background from "../../assets/backgrounds/background3.jpg";

const mapCenter = [-45.86168350577915, -67.5188749673741]; // Comodoro Rivadavia, Argentina

const View = () => {
    const [featureCollection, setFeatureCollection] = useState({features:[]});

    const onInputLoaded = data => {
        const featureCollection = JSON.parse(data);
        if(isValidGeoJSON(featureCollection)){
            setFeatureCollection(featureCollection);
        }else{
            alert("El archivo no contiene un GeoJSON válido");
        }
    };

    return(
        <MainView background={background}>
            <Grid container spacing={2} direction="row">
                <Grid size={4}>
                    <Box sx={{height: "100%", width: "100%", display: "flex", alignItems: "center", justifyContent: "center"}}>
                        <DropZone onDrop={onInputLoaded} />
                    </Box>
                </Grid>
                <Grid size={8}>
                    <Map 
                        mapCenter={mapCenter}
                        featureCollection={featureCollection}/>
                </Grid>
            </Grid>
        </MainView>
    )
};

export default View;