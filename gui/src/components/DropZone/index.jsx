import { useCallback } from 'react';
import { Box, Typography } from '@mui/material';
import { useDropzone } from 'react-dropzone';
import { getFileFormat } from '../../model/utils';
import { useFileIdsContext } from "../../context/FileIds";
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

const DropzoneComponent = ({ onDrop, onError }) => { // expects onDrop(data, format), where format is "json" or "csv"
    
    const { fileIds, uploadFile, removeFile } = useFileIdsContext();
    const preloader = usePreloader();

    const onDropAccepted = useCallback((acceptedFiles) => {
        const file = acceptedFiles[0]; // only one file allowed
        if (!file) return;

        const reader = new FileReader();
        reader.onload = async () => {
            // Read file content
            const fileContent = reader.result;
            const format = getFileFormat(fileContent);

            // Send loaded file to parent component for using
            onDrop(fileContent, format);

            // Check if file is already uploaded, if so, try to remove it first
            try{
                const id = { // map file types to context ids
                    '.csv': fileIds.em_file,
                    '.geojson': fileIds.features_file
                }
                if(id[format]) {
                    console.log("Removing previous file with id:", id[format], "and format:", format);
                    await removeFile(id[format], format);
                }
            } catch(err) {
                console.error("Error removing previous file:", err);
            }

            // Send file to backend for storage and further processing
            try {
                if(format === '.csv' || format === '.geojson')
                    await uploadFile(file);
                else
                    console.error("Unsupported file format for upload");
            } catch (err) {
                console.error("Upload failed:", err);
                if (onError) 
                    onError("Error al cargar el archivo");
            }
            preloader(false);
        };
        reader.readAsText(file);
        preloader(true);
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
