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

    return (
        <MainView background={background}>
            <Grid container spacing={2} direction="column">
                <Grid item>
                    <Grid container spacing={2} direction="row">
                        <Grid item size={3}>    
                            <Controls />
                        </Grid>
                        <Grid item size={9}>
                            <EDTable featureCollection={featureCollection}/>
                            <GWTable featureCollection={featureCollection}/>
                        </Grid>
                    </Grid>
                </Grid>
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