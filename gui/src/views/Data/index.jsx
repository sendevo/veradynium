import {
    Grid
} from "@mui/material";
import MainView from "../../components/MainView";
import Controls from "../../components/Controls";
import NodesTable from "../../components/NodesTable";
import { useModelContext } from "../../context/Model";
import background from "../../assets/backgrounds/background3.jpg";


const View = () => {

    const { files } = useModelContext();

    const featureCollection = files.features.content || { features: [] };

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