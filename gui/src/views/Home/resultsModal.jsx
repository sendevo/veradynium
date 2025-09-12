import { Grid, Typography } from "@mui/material";

const ResultsModal = (result) => {
    return (
        <Grid>
            <Typography sx={{fontWeight:"bold"}}>Punto 1</Typography>
            <Typography>lat: {result.point1.lat}</Typography>
            <Typography>lon: {result.point1.lng}</Typography>
            <Typography>elev: {result.point1.height_m} m</Typography>

            <Typography sx={{fontWeight:"bold", mt:1}}>Punto 2</Typography>
            <Typography>lat: {result.point2.lat}</Typography>
            <Typography>lon: {result.point2.lng}</Typography>
            <Typography>elev: {result.point2.height_m} m</Typography>
            
            <Typography sx={{mt:1}}><b>Distancia:</b> {result.distance_m} m</Typography>

            <Typography sx={{mt:1}}><b>Línea de vista:</b> {result.line_of_sight ? "Si" : "No"}</Typography>
        </Grid>
    );
};

export default ResultsModal;