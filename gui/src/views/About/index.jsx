import { Link, Typography } from "@mui/material";
import MainView from "../../components/MainView";
import { FaLink } from "react-icons/fa";

const View = () => (
    <MainView title="">
        <Typography variant="h5" gutterBottom>
            <Link href="https://github.com/sendevo/veradynium" target="_blank" rel="noopener"
            color="inherit" underline="hover">
                <FaLink/> Documentación
            </Link>
        </Typography>
    </MainView>
);

export default View;