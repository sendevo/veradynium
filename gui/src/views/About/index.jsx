import { Link, Typography } from "@mui/material";
import { useTranslation } from "react-i18next";
import MainView from "../../components/MainView";
import { FaLink } from "react-icons/fa";

const View = () => {
    
    const { t } = useTranslation("about");

    return (
        <MainView title="">
            <Typography variant="h4" gutterBottom>
                Veradynium v1.0.0
            </Typography>
            <Typography variant="h5" gutterBottom>
                {t("about")}
            </Typography>
            <br/>
            <Typography variant="h5" gutterBottom>
                <Link href="https://github.com/sendevo/veradynium" target="_blank" rel="noopener"
                color="inherit" underline="hover">
                    {t("documentation")}
                    <FaLink style={{marginLeft: "10px"}}/> 
                </Link>
            </Typography>
        </MainView>
    );
};

export default View;