import { 
    Grid,
    Typography 
} from "@mui/material";
import { useTranslation } from "react-i18next";
import Input from "../Inputs/Input";


const PointConfig = ({points, setPoints}) => {

    const { t } = useTranslation("controls");

    const handleSetAltitude = (point, value) => {
        setPoints(prevPoints => {
            const newPoints = [...prevPoints];
            newPoints[point] = {
                ...newPoints[point],
                height_m: Number(value)
            };
            return newPoints;
        });
    };
    
    return ( // Three inputs show lat, lng and elevation
        points?.length > 0 &&
            <Grid container spacing={2} sx={{border: "1px solid #555", padding: 2, borderRadius: 2, marginTop: 2}}>
                {points.map((point, index) => (
                    <Grid item xs={6}>
                        <Typography sx={{fontWeight: "bold", pt: 1}}>
                            {t("point")+" "+(index+1)}
                        </Typography>
                        <Typography sx={{marginBottom: 1, fontSize: 14, m:0}}>
                            {"lat.="+point.lat.toFixed(2)+", lon.="+point.lng.toFixed(2)}
                        </Typography>
                        <Input
                            name={`point-${index}-elevation`}
                            label={t("elevation")}
                            type="number"
                            value={point.height_m}
                            unit="m"
                            onChange={(e) => handleSetAltitude(index, e.target.value)}/>
                    </Grid>
                ))}
            </Grid>
    );
};

export default PointConfig;