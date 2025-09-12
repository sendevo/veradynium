import { useState } from "react";
import { 
    Typography,
    Modal as MuiModal,
    Box, 
} from "@mui/material";
import LineChart from "./lineChart";

const style = {
  position: 'absolute',
  top: '50%',
  left: '50%',
  transform: 'translate(-50%, -50%)',
  width: '70%',
  backgroundColor: '#000000CC',
  border: '1px solid #000',
  boxShadow: 5,
  p: 4
};

const requiredProps = ["point1", "point2", "distance_m", "line_of_sight", "terrain_profile_m"];

const ResultsModal = props => {
    
    const { result } = props;

    const [open, setOpen] = useState(true);
    const handleClose = () => setOpen(false);

    if(!result){
        console.warn("ResultsModal: No result provided");
        return null;
    }

    for(const prop of requiredProps){
        if(!(prop in result)){
            console.warn(`ResultsModal: Missing required prop '${prop}' in result`);
            return null;
        }
    }

    return (
        <MuiModal open={open} onClose={handleClose} aria-labelledby="modal-title" aria-describedby="modal-description">
            <Box sx={style}>
                <Typography sx={{fontSize: 18, fontWeight:"bold", mb:2}}>Cálculo de línea de vista (LOS)</Typography>

                <Typography sx={{fontWeight:"bold"}}>Punto 1</Typography>
                <Typography>lat: {result.point1.lat}, lon: {result.point1.lng}</Typography>
                <Typography>elev.: {result.point1.height_m} m</Typography>

                <Typography sx={{fontWeight:"bold", mt:1}}>Punto 2</Typography>
                <Typography>lat: {result.point2.lat}, lon: {result.point2.lng}</Typography>
                <Typography>elev: {result.point2.height_m} m</Typography>
                
                <Typography sx={{mt:1}}><b>Distancia:</b> {result.distance_m} m</Typography>

                <Typography sx={{mt:1}}><b>Línea de vista:</b> {result.line_of_sight ? "Si" : "No"}</Typography>

                <Typography sx={{fontWeight:"bold", mt:2, mb:3}}>Perfil de terreno (m)</Typography>
                <LineChart data={result.terrain_profile_m}/>
            </Box>
        </MuiModal>
    );
};

export default ResultsModal;