import {
    Grid
} from "@mui/material";
import MainView from "../../components/MainView";
import Controls from "../../components/Controls";
import NodesTable from "../../components/NodesTable";
import { useFilesContext } from "../../context/Files";
import useAnalysis from "../../hooks/useAnalysis";
import background from "../../assets/backgrounds/background3.jpg";


const View = () => {

    const { files } = useFilesContext();
    const { networkResult } = useAnalysis();

    const featureCollection = networkResult || files.features.content || { features: [] };

    return (
        <MainView background={background}>
            <Grid container spacing={2} direction="row" sx={{height:"75vh"}}>
                <Grid size={3}>    
                    <Controls />
                </Grid>
                <Grid size={9}>
                    <NodesTable featureCollection={featureCollection}/>
                </Grid>
            </Grid>
        </MainView>
    );
};

export default View;