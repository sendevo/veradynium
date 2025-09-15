import { useState, useCallback } from "react";
import { api } from '../../model/constants';
import { 
    fetchWithTimeout,
    csvToArray, 
    getFileFormat,
    chunkedMax, 
    chunkedMin, 
    normalizeElevation 
} from "../../model/utils";


const useFiles = (toast, preloader) => {
    
    const [files, setFiles] = useState({
        elevation_map: {
            id: null,
            content: null
        },
        features: {
            id: null,
            content: null
        }
    });

    // Upload file to backend
    const uploadFile = useCallback(async file => {
        preloader(true);
        try{
            const formData = new FormData();
            formData.append("file", file);
            const extension = "." + file.name.split('.').pop().toLowerCase();
            const res = await fetchWithTimeout(api("/api/upload"), {
                method: "POST",
                body: formData,
            });
            
            if (!res.ok) {
                preloader(false);
                toast("Error al cargar el archivo", "error");
                throw new Error("Failed to upload file");
            }
            
            const {
                upload_id, // id to reference the file in server side for future requests
                data // if file format is .nc, server will return elevation data as array
            } = await res.json();
            
            let content = null;
            let file_type = null;
            if(extension === ".nc"){
                file_type = "elevation_map";
                content = data; // elevation data as array
            }else{ // geojson or csv -> file needs to be parsed in client side
                const reader = new FileReader();
                reader.onload = () => {
                    const fileContent = reader.result;
                    const format = getFileFormat(fileContent);
                    if(format === ".geojson"){
                        try {
                            const fc = JSON.parse(fileContent);
                            file_type = "features";
                            content = fc;
                        } catch (err) {
                            console.error("Error parsing GeoJSON:", err);
                            toast("Error al parsear GeoJSON", "error");
                        }
                    } else { 
                        if(format === ".csv"){
                            file_type = "elevation_map";
                            // parse CSV
                            const arr = csvToArray(fileContent, { withHeaders: false, delimiter: ",", maxRows: -1 });
                            const alts = arr.map(p => p[2]);
                            const normalized = normalizeElevation(arr, chunkedMin(alts), chunkedMax(alts));
                            content = normalized;
                        } else {
                            toast("Formato de archivo no soportado", "error");
                        }
                    } 
                    if(content){
                        toast(`Archivo ${file.name} cargado exitosamente`, "success");
                        setFiles(prev => ({
                            ...prev,
                            [file_type]: {
                                id: upload_id,
                                content
                            }
                        }));
                    }
                    preloader(false);
                };
                reader.readAsText(file);
            }
        } catch(err) {
            console.error("Upload error:", err);
            toast("Error al cargar el archivo", "error");
            throw err;
        }
    }, [files, setFiles]);

    // Remove file from state (doesn't delete from backend)
    const removeFile = useCallback(async (upload_id, format) => {
        const extension = format === '.geojson' ? '.json' : format; // server uses .json for geojson files
        try{
            const res = await fetchWithTimeout(api(`/api/delete`), {
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify({upload_id, extension})
            });
            if (!res.ok)
                throw new Error("Failed to delete file from server");
            const newFile = { id: null, content: null };
            if(format === ".geojson"){
                setFiles(prev => ({ ...prev, features: newFile }));
            } else {
                if(format === ".csv" || format === ".nc") {
                    setFiles(prev => ({ ...prev, elevation_map: newFile }));
                } else { 
                    toast("Formato de archivo no soportado", "error");
                }
            }
            toast("Archivo eliminado exitosamente", "success");
        }catch(err){
            console.error("Delete error:", err);
        }

    }, [files, setFiles]);

    return { files, uploadFile, removeFile  };
}

export default useFiles;