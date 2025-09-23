import { useCallback } from 'react';
import { Grid, Box, Typography } from '@mui/material';
import { useTranslation } from "react-i18next";
import { useDropzone } from 'react-dropzone';
import MenuButtons from "./menuButtons";
import { useModelContext } from "../../context/Model";

const dropzoneStyle = {
    verticalAlign: 'middle',
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center',
    height: "100%",
    width: "100%",
    borderWidth: 2,
    borderRadius: 10,
    borderColor: 'white',
    borderStyle: 'dashed',
    backgroundColor: 'rgba(0,0,0,0.6)',
    color: 'white',
    padding: 20,
    margin: 'auto',
    textAlign: 'center'
};

const DropZoneArea = ({ onDrop, onError }) => {

    const { t } = useTranslation("controls");

    const onDropAccepted = useCallback((acceptedFiles) => {
        const file = acceptedFiles[0];
        if (!file) return;
        onDrop(file);
    }, [onDrop, onError]);

    const { getRootProps, getInputProps } = useDropzone({
        onDropAccepted,
        accept: {
            'text/csv': ['.csv'], // Elevation data
            'application/json': ['.json'], // GeoJSON
            'application/x-netcdf': ['.nc'] // NetCDF (elevation data, requires backend support)
        },
        multiple: false
    });

    return (
        <Box {...getRootProps()} style={dropzoneStyle}>
            <input {...getInputProps()} />
            <Typography style={{fontSize: 18}}>{t("dropzone_text")}</Typography>
        </Box>
    );
};


const Controls = ({
    handleResetPoints,
    handleComputeLOS,
    evalNetworkAction,
    runSolverAction,
    points
}) => {

    const { t } = useTranslation("controls");

    const {
        files,
        uploadFile,
        removeFile
    } = useModelContext();

    const hasElevation = files.elevation_map.content && files.elevation_map.content.length > 0;
    const hasFeatures = files.features.content && files.features.content.features && files.features.content.features.length > 0;

    const handleUploadFile = file => {
        uploadFile(file);
    };

    const handleRemoveElevation = () => {
        removeFile(files.elevation_map.id, ".csv");
    };

    const handleRemoveFeatures = () => {
        removeFile(files.features.id, ".geojson");
    };

    return (
        <Grid container direction={"column"} spacing={2} sx={{height:"100%"}}>
            <Grid size={"grow"}>
                <DropZoneArea
                    onDrop={handleUploadFile} 
                    onError={message => toast(message, "error")}/>
            </Grid>
            {(hasElevation || hasFeatures) && 
                <Grid>
                    <Typography>{t("files_status")}:</Typography>
                
                    {hasElevation && 
                        <>
                            {files.elevation_map.id ? 
                                <Typography sx={{fontSize: 12}}>{t("elevation_map_uploaded")}</Typography>
                                :
                                <Typography sx={{fontSize: 12}}>{t("elevation_map_local")}</Typography>
                            }
                        </>
                    }
                    {hasFeatures &&  
                        <>
                            {files.features.id ?
                                <Typography sx={{fontSize: 12}}>{t("features_uploaded")}</Typography>
                                :
                                <Typography sx={{fontSize: 12}}>{t("features_local")}</Typography>
                            }
                        </>
                    }
                </Grid>
            }

            <MenuButtons
                hasFeatures={hasFeatures}
                hasElevation={hasElevation}
                hasUploadedFiles={files.elevation_map.id && files.features.id}
                handleComputeLOS={handleComputeLOS}
                evalNetworkAction={evalNetworkAction}
                runSolverAction={runSolverAction}
                points={points}
                handleResetPoints={handleResetPoints}
                handleRemoveFeatures={handleRemoveFeatures}
                handleRemoveElevation={handleRemoveElevation}/>
        </Grid>
    );
};

export default Controls;
