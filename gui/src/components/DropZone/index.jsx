import { useCallback } from 'react';
import { Box, Typography } from '@mui/material';
import { useDropzone } from 'react-dropzone';
import usePreloader from '../../hooks/usePreloader';

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

const DropzoneComponent = ({ onDrop, onError }) => {
    
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


export default DropzoneComponent;
