import { Typography, Modal, Box } from "@mui/material";
import { useTranslation } from "react-i18next";

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

const requiredProps = ["num_end_devices", "num_gateways", "connected_end_devices", "disconnected_end_devices", "total_distance"];

const NetworkResultsModal = props => {
    
    const { result, open, onClose } = props;
    const { t } = useTranslation("network_result_modal");

    if(!result){
        console.warn("NetworkResultsModal: No result provided");
        return null;
    }

    for(const prop of requiredProps){
        if(!(prop in result)){
            console.warn(`NetworkResultsModal: Missing required prop '${prop}' in result`);
            return null;
        }
    }

    const connectivityProportion = (result.num_end_devices > 0 
        ? (result.connected_end_devices / result.num_end_devices) * 100 
        : 0).toFixed(2);

    return (
        <Modal open={open} onClose={onClose} aria-labelledby="modal-title" aria-describedby="modal-description">
            <Box sx={style}>
                <Typography sx={{fontSize: 18, fontWeight:"bold", mb:2}}>{t("title")}</Typography>

                <Typography>{t("total_devices")}: {result.num_end_devices}</Typography>
                <Typography>{t("gateways")}: {result.num_gateways}</Typography>
                <Typography>{t("connected_devices")}: {result.connected_end_devices}</Typography>
                <Typography>{t("disconnected_devices")}: {result.disconnected_end_devices}</Typography>
                <Typography>{t("connectivity_percentage")}: {connectivityProportion} %</Typography>
                <Typography>{t("total_link_distance")}: {result.total_distance.toFixed(2)} m</Typography>
                
            </Box>
        </Modal>
    );
};

export default NetworkResultsModal;