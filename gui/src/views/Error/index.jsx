import { 
    Typography, 
    Button, 
    Box,
    TextField,
    Grid
} from "@mui/material";
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

    const handleReport = () => {
        toast("Error reportado", "success");
        onReport();
    };

    const handleReset = () => {
        toast("Aplicación reiniciada", "success");
        onReset();
    };

    return(
        <MainView title={"Ocurrió un error crítico"} >
            <Typography sx={messageStyle} mb={2}>
                La aplicación ha detectado un error crítico y no puede continuar ejecutándose. Nuestro equipo está trabajando para resolverlo lo antes posible.
            </Typography>
            <img src={image} style={{
                width: "100%",
                top: "50%",
                borderRadius: "10%"
            }}/>
            <Typography sx={messageStyle} mt={2}>
                Vuelva a intentarlo nuevamente reiniciando la aplicación o envíe un reporte para ayudarnos a encontrar el problema.
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
                            Enviar reporte
                    </Button>
                </Grid>
                <Grid item>
                    <Button 
                        onClick={handleReset}
                        variant={"contained"}
                        color={"primary"}>
                            Reiniciar aplicación
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