import {
    Box,
    Grid,
    Typography
} from "@mui/material";
import { useTranslation } from "react-i18next";
import MainView from "../../components/MainView";
import Controls from "../../components/Controls";
import { EDTable, GWTable } from "../../components/NodesTable";
import Histogram from "../../components/Charts/barChart.jsx";
import { useModelContext } from "../../context/Model";
import background from "../../assets/backgrounds/background3.jpg";


const View = () => {

    const { model } = useModelContext();
    const { t } = useTranslation("data_view");

    const featureCollection = model.features.content || { features: [] };
    const result = featureCollection.properties || null;

    const distHistogramBinSize = model.features?.content?.properties?.distance_histogram_bin_size || 1;
    const distHistogramValues = model.features?.content?.properties?.distance_histogram || [];
    const distData = distHistogramValues.map((count, i) => ({
        range: `${(i * distHistogramBinSize)}–${((i + 1) * distHistogramBinSize)} m`,
        count,
    }));

    const sfHistogramBinSize = model.features?.content?.properties?.sf_histogram_bin_size || 1;
    const sfHistogramValues = model.features?.content?.properties?.sf_histogram || [];
    const sfData = sfHistogramValues.map((count, i) => ({
        range: `SF ${(i * sfHistogramBinSize+7)}`,
        count,
    }));

    const enableData = Array.isArray(featureCollection.features) && featureCollection.features.length > 0;

    return (
        <MainView background={background}>
            <Grid container spacing={2} direction="column">
                <Grid item>
                    <Grid container spacing={2} direction="row">
                        <Grid item size={3}>    
                            <Controls />
                        </Grid>
                        {enableData ? 
                            <Grid item size={9}>
                                <EDTable featureCollection={featureCollection}/>
                                <GWTable featureCollection={featureCollection}/>
                            </Grid>
                            :
                            <Box sx={{height:"65vh", display:"flex", justifyContent:"center", alignItems:"center"}}>
                                <Typography variant="h6" align="center" color="white" sx={{mt:2}}>
                                    {t("empty_table_1")} <br/> {t("empty_table_2")}
                                </Typography>
                            </Box>
                        }
                    </Grid>
                </Grid>
                {enableData && result &&
                    <Grid item >
                        <Typography sx={{fontWeight:"bold"}}>{t("network_statistics")}:</Typography>
                        <Typography>{t("total_devices")}: {result.num_end_devices}</Typography>
                        <Typography>{t("gateways")}: {result.num_gateways}</Typography>
                        <Typography>{t("connected_devices")}: {result.connected_end_devices}</Typography>
                        <Typography>{t("disconnected_devices")}: {result.disconnected_end_devices}</Typography>
                        <Typography>{t("connectivity_percentage")}: {result.coverage} %</Typography>
                        <Typography>{t("energy_consumption_estimate")}: {result.total_energy_consumption ? result.total_energy_consumption.toFixed(2) + " Wh" : t("not_available")}</Typography>
                    </Grid>
                }
                {distHistogramValues.length > 0 &&
                    <Grid item sx={{mt:2}}>
                        <Typography sx={{fontWeight:"bold"}}>{t("distance_histogram")}:</Typography>
                        <Histogram 
                            binSize={distHistogramBinSize}
                            data={distData}/>
                    </Grid>
                }
                {sfHistogramValues.length > 0 &&
                    <Grid item>
                        <Typography sx={{fontWeight:"bold"}}>{t("sf_histogram")}</Typography>
                        <Histogram 
                            binSize={sfHistogramBinSize}
                            data={sfData}/>
                    </Grid>
                }
            </Grid>
        </MainView>
    );
};

export default View;