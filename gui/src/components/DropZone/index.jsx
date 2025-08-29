import { useCallback } from 'react';
import { Box, Typography } from '@mui/material';
import { useDropzone } from 'react-dropzone';
import { getFileFormat } from '../../model/utils';

const dropzoneStyle = {
    verticalAlign: 'middle',
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center',
    height: "75vh",
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

const DropzoneComponent = ({ onDrop }) => { // expects onDrop(data, format), where format is "json" or "csv"
    
    const onDropAccepted = useCallback((acceptedFiles) => {
        acceptedFiles.forEach((file) => {
            const reader = new FileReader();
            reader.onload = () => {
                const fileContent = reader.result;
                const format = getFileFormat(fileContent);
                onDrop(fileContent, format);
            };
            reader.readAsText(file);
        });
    }, [onDrop]);

    const { getRootProps, getInputProps } = useDropzone({
        onDropAccepted,
        accept: 'application/json',
        multiple: false
    });

    return (
        <Box {...getRootProps()} style={dropzoneStyle}>
            <input {...getInputProps()} />
            <Typography style={{fontSize: 28}}>Arrastrar y soltar archivos aquí o seleccione desde su directorio</Typography>
        </Box>
    );
};


export default DropzoneComponent;
