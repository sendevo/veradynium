import { Typography, Modal, Box } from "@mui/material";
import { useTranslation } from "react-i18next";
import Histogram from "../../components/Charts/barChart.jsx";

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

const requiredProps = ["num_end_devices", "num_gateways", "connected_end_devices", "disconnected_end_devices", "total_distance", "distance_histogram_bin_size", "distance_histogram"];

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

    /*
    const distHistogramBinSize =result.distance_histogram_bin_size || 1;
    const distHistogramValues = result.distance_histogram || [];
    const distData = distHistogramValues.map((count, i) => ({
        range: `${(i * distHistogramBinSize)}–${((i + 1) * distHistogramBinSize)} m`,
        count,
    }));
    */

    const sfHistogramBinSize = result.sf_histogram_bin_size || 1;
    const sfHistogramValues = result.sf_histogram || [];
    const sfData = sfHistogramValues.map((count, i) => ({
        range: `SF ${(i * sfHistogramBinSize+7)}`,
        count,
    }));

    return (
        <Modal open={open} onClose={onClose} aria-labelledby="modal-title" aria-describedby="modal-description">
            <Box sx={style}>
                <Typography sx={{fontSize: 18, fontWeight:"bold", mb:2}}>{t("title")}</Typography>

                <Typography>{t("total_devices")}: {result.num_end_devices}</Typography>
                <Typography>{t("gateways")}: {result.num_gateways}</Typography>
                <Typography>{t("connected_devices")}: {result.connected_end_devices}</Typography>
                <Typography>{t("disconnected_devices")}: {result.disconnected_end_devices}</Typography>
                <Typography>{t("connectivity_percentage")}: {result.coverage} %</Typography>
                {/*<Typography>{t("total_link_distance")}: {result.total_distance.toFixed(2)} m</Typography>*/}
                <Typography>{t("energy_consumption_estimate")}: {result.total_energy_consumption ? result.total_energy_consumption.toFixed(2) + " Wh" : t("not_available")}</Typography>
                <Typography sx={{fontWeight:"bold", mt:2, mb:3}}>{t("sf_histogram")}:</Typography>
                {result.distance_histogram.length > 0 && 
                    <Histogram 
                        binSize={sfHistogramBinSize} 
                        data={sfData}/>
                }
            </Box>
        </Modal>
    );
};

export default NetworkResultsModal;