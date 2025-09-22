import { useState, useEffect, useCallback } from 'react';
import { Button, Grid, Box, Typography } from '@mui/material';
import { useDropzone } from 'react-dropzone';
import MenuButtons from "./menuButtons";
import { useFilesContext } from "../../context/Files";

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
            <Typography style={{fontSize: 18}}>Arrastrar y soltar archivos aquí o seleccione desde su directorio</Typography>
        </Box>
    );
};


const Controls = ({
    onAddElevation,
    onAddFeatures,
    onRemoveElevation,
    onRemoveFeatures,
    handleResetPoints,
    handleComputeLOS,
    evalNetworkAction,
    runSolverAction,
    points
}) => {

    const {
        files,
        uploadFile,
        removeFile
    } = useFilesContext();

    const [hasElevation, setHasElevation] = useState(false);
    const [hasFeatures, setHasFeatures] = useState(false);

    useEffect(() => { // On files uploaded
        const elevationData = files.elevation_map.content || [];
        const featureCollection = files.features.content || { features: [] };
        onAddElevation(elevationData, false);
        onAddFeatures(featureCollection);
        setHasFeatures(featureCollection.features && featureCollection.features.length > 0);
        setHasElevation(elevationData.length > 0);
    }, [files]);

    const handleUploadFile = file => {
        const extension = "." + file.name.split(".").pop().toLowerCase();
        if(extension === ".json")
            onAddElevation(null, true); // Reset network connections
        uploadFile(file);
    };

    const handleRemoveElevation = () => {
        removeFile(files.elevation_map.id, ".csv");
        onRemoveElevation();
    };

    const handleRemoveFeatures = () => {
        removeFile(files.features.id, ".geojson");
        onRemoveFeatures();
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
                    <Typography>Estado de los archivos:</Typography>
                
                    {hasElevation && 
                        <>
                            {files.elevation_map.id ? 
                                <Typography sx={{fontSize: 12}}>Mapa de elevación cargado</Typography>
                                :
                                <Typography sx={{fontSize: 12}}>Mapa de elevación en modo local</Typography>
                            }
                        </>
                    }
                    {hasFeatures &&  
                        <>
                            {files.features.id ?
                                <Typography sx={{fontSize: 12}}>Geometrías cargadas</Typography>
                                :
                                <Typography sx={{fontSize: 12}}>Geometrías en modo local</Typography>
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
