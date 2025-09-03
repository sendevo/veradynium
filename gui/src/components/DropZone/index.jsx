import { useCallback } from 'react';
import { Box, Typography } from '@mui/material';
import { useDropzone } from 'react-dropzone';
import { getFileFormat } from '../../model/utils';
import { api } from '../../model/constants';

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

const DropzoneComponent = ({ onDrop, onError }) => { // expects onDrop(data, format), where format is "json" or "csv"
    
    const onDropAccepted = useCallback((acceptedFiles) => {
        const file = acceptedFiles[0]; // only one file allowed
        if (!file) return;

        const reader = new FileReader();
        reader.onload = async () => {
            // 1. Read file content
            const fileContent = reader.result;
            const format = getFileFormat(fileContent);

            // 2. Send raw file to backend
            const formData = new FormData();            
            formData.append("file", file);
            try {
                const res = await fetch(api("/api/upload"), {
                    method: "POST",
                    body: formData
                });
                const data = await res.json();
                console.log("Upload response:", data);
            } catch (err) {
                console.error("Upload failed:", err);
                if (onError) 
                    onError("Error al cargar el archivo");
            } finally {
                // 3. Call onDrop prop with file content and format
                onDrop(fileContent, format);
            }
        };
        reader.readAsText(file);
    }, [onDrop]);

    const { getRootProps, getInputProps } = useDropzone({
        onDropAccepted,
        accept: {
            'application/json': ['.json', '.geojson'],
            'text/csv': ['.csv', '.txt']
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
