import {
    Grid
} from "@mui/material";
import MainView from "../../components/MainView";
import background from "../../assets/backgrounds/background3.jpg";


const View = () => {

    return (
        <MainView background={background}>
            <Grid container spacing={2} direction="row" sx={{height:"75vh"}}>
                <Grid size={3}>
                    
                </Grid>
                <Grid size={9}>
                    
                </Grid>
            </Grid>
        </MainView>
    );
};

export default View;