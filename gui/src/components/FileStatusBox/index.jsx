import { Box, Typography } from "@mui/material";
import { useTranslation } from "react-i18next";
import { useModelContext } from "../../context/Model";

const boxContainerStyle = {
    position: 'absolute',
    bottom: 10,
    right: 10,
    backgroundColor: 'rgba(0, 0, 0, 0.7)',
    padding: '8px 12px',
    borderRadius: 3,
    color: 'white',
    zIndex: 999
};

const FileStatusBox = () => {

    const { model } = useModelContext();

    const { t } = useTranslation("controls");
    
    const hasElevation = model.elevation_map.content && model.elevation_map.content.length > 0;
    const hasFeatures = model.features.content && model.features.content.features && model.features.content.features.length > 0;
    
    return(
        (hasElevation || hasFeatures) && 
            <Box sx={boxContainerStyle}>
                <Typography sx={{fontSize:14}}>{t("files_status")}:</Typography>
            
                {hasElevation && 
                    <>
                        {model.elevation_map.id ? 
                            <Typography sx={{fontSize: 12}}>{t("elevation_map_uploaded")}</Typography>
                            :
                            <Typography sx={{fontSize: 12}}>{t("elevation_map_local")}</Typography>
                        }
                    </>
                }
                {hasFeatures &&  
                    <>
                        {model.features.id ?
                            <Typography sx={{fontSize: 12}}>{t("features_uploaded")}</Typography>
                            :
                            <Typography sx={{fontSize: 12}}>{t("features_local")}</Typography>
                        }
                    </>
                }
            </Box>
    );
};

export default FileStatusBox;