import { Typography, Modal, Box } from "@mui/material";
import { useTranslation } from "react-i18next";
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

const requiredProps = ["point1", "point2", "distance_m", "line_of_sight", "terrain_profile_elev_m", "terrain_profile_dist_m"];

const LOSResultsModal = props => {
    
    const { result, open, onClose } = props;

    const { t } = useTranslation("los_results_modal");

    if(!result){
        console.warn("LOSResultsModal: No result provided");
        return null;
    }

    for(const prop of requiredProps){
        if(!(prop in result)){
            console.warn(`LOSResultsModal: Missing required prop '${prop}' in result`);
            return null;
        }
    }

    return (
        <Modal open={open} onClose={onClose} aria-labelledby="modal-title" aria-describedby="modal-description">
            <Box sx={style}>
                <Typography sx={{fontSize: 18, fontWeight:"bold", mb:2}}>{t("title")}</Typography>

                <Typography sx={{fontWeight:"bold"}}>{t("point_1")}</Typography>
                <Typography>{t("lat")}: {result.point1.lat}, {t("lon")}: {result.point1.lng}</Typography>
                <Typography>{t("elev")}: {result.point1.height_m} m</Typography>

                <Typography sx={{fontWeight:"bold", mt:1}}>{t("point_2")}</Typography>
                <Typography>{t("lat")}: {result.point2.lat}, {t("lon")}: {result.point2.lng}</Typography>
                <Typography>{t("elev")}: {result.point2.height_m} m</Typography>
                
                <Typography sx={{mt:1}}><b>{t("distance")}:</b> {result.distance_m} m</Typography>

                <Typography sx={{mt:1}}><b>{t("line_of_sight")}:</b> {result.line_of_sight ? t("yes") : t("no")}</Typography>

                <Typography sx={{mt:1}}><b>{t("line_of_sight_fresnel_60pct")}:</b> {result.line_of_sight_fresnel_60pct ? t("yes") : t("no")}</Typography>

                <Typography sx={{fontWeight:"bold", mt:2, mb:3}}>{t("terrain_profile")} (m):</Typography>
                <LineChart 
                    elev_data={result.terrain_profile_elev_m} 
                    dist_data={result.terrain_profile_dist_m}/>
            </Box>
        </Modal>
    );
};

export default LOSResultsModal;