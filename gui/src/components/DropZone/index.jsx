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
        const file = acceptedFiles[0];
        if (!file) return;

        const extension = file.name.split('.').pop().toLowerCase();

        if (extension === "csv" || extension === "geojson") {
            const reader = new FileReader();
            reader.onload = () => {
                (async () => {
                    const fileContent = reader.result;
                    const format = "." + extension;

                    onDrop(fileContent, format);

                    try {
                        const id = {
                            '.csv': fileIds.em_file,
                            '.geojson': fileIds.features_file
                        };
                        if (id[format]) {
                            await removeFile(id[format], format);
                        }
                    } catch (err) {
                        console.error("Error removing previous file:", err);
                    }

                    try {
                        await uploadFile(file);  // still upload to server
                    } catch (err) {
                        console.error("Upload failed:", err);
                        if (onError) onError("Error al cargar el archivo");
                    }
                    preloader(false);
                })();
            };
            reader.readAsText(file);
            preloader(true);
        } else if (extension === "nc") {
            const format = ".nc";
            onDrop(null, format);

            (async () => {
                try {
                    await uploadFile(file);  // send raw binary
                } catch (err) {
                    console.error("Upload failed:", err);
                    if (onError) onError("Error al cargar el archivo");
                }
                preloader(false);
            })();
        } else {
            console.error("Unsupported file format for upload");
        }
    }, [onDrop, fileIds, uploadFile, removeFile, onError, preloader]);


    const { getRootProps, getInputProps } = useDropzone({
        onDropAccepted,
        accept: {
            'text/csv': ['.csv'],
            'application/geo+json': ['.geojson'],
            'application/x-netcdf': ['.nc'],   // NetCDF
            'application/octet-stream': ['.nc'] // fallback
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
