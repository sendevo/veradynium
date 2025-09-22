import { 
    Typography, 
    Button, 
    Box,
    TextField,
    Grid
} from "@mui/material";
import { useTranslation } from "react-i18next";
import useToast from "../../hooks/useToast";
import MainView from "../../components/MainView";
import image from "../../assets/working_monkey.jpg";

const messageStyle = {
    fontSize: "15px",
    color: "rgb(100,100,100)"
};

const errorBlockStyle = {
    justifyContent: "center", 
    display: "flex",
    gap: "20px",
    width: "100%",
    paddingTop: "20px",
};

const View = ({errorMessage, onReset, onReport}) => {

    const toast = useToast();

    const { t } = useTranslation("error");

    const handleReport = () => {
        toast("Error reportado", "success");
        onReport();
    };

    const handleReset = () => {
        toast("Aplicación reiniciada", "success");
        onReset();
    };

    return(
        <MainView title={t("critical_error")} >
            <Typography sx={messageStyle} mb={2}>
                {t("error_msg_1")}
            </Typography>
            <img src={image} style={{
                width: "100%",
                top: "50%",
                borderRadius: "10%"
            }}/>
            <Typography sx={messageStyle} mt={2}>
                {t("error_msg_2")}
            </Typography>
            <Grid 
                container 
                direction={"row"}
                mt={3}
                mb={3}
                justifyContent={"space-evenly"}
                >
                <Grid item>
                    <Button 
                        onClick={handleReport}
                        variant={"contained"}
                        color={"primary"}>
                            {t("send_report")}
                    </Button>
                </Grid>
                <Grid item>
                    <Button 
                        onClick={handleReset}
                        variant={"contained"}
                        color={"primary"}>
                            {t("restart_app")}
                    </Button>
                </Grid>
            </Grid>
            <Box sx={errorBlockStyle}>
                <TextField
                    label={"Crash dump"}
                    sx={{
                        "& .MuiInputBase-input.Mui-disabled": {
                          WebkitTextFillColor: "#FF0000",
                        },
                    }}
                    value={errorMessage}
                    error
                    multiline
                    rows={15}
                    fullWidth
                    variant={"outlined"}
                    disabled
                    inputProps={{
                        style: {
                            fontFamily: "monospace",
                            fontSize: "13px"
                        }
                    }}
                />
            </Box>
        </MainView>
    );
};

export default View;