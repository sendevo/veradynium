import { useState, useCallback } from "react";
import { useTranslation } from "react-i18next";
import { api } from '../../model/constants';
import { 
    fetchWithTimeout,
    getFileFormat,
    csvToElevation,
    readFile
} from "../../model/utils";
import useToast from "../useToast";
import usePreloader from "../usePreloader";


const defaultModel = { // System uses elevation map and FeatureCollection files
  elevation_map: { id: null, content: null },
  features: { id: null, content: null },
};

const extensionToType = {
    ".csv": "elevation_map",
    ".nc": "elevation_map",
    ".geojson": "features",
    ".json": "features"
};

const useModel = () => {
    const toast = useToast();
    const preloader = usePreloader();
    const { t } = useTranslation("model");
    
    const [model, setModel] = useState(defaultModel);

    const setModelContent = (type, id, content) => setModel(prev => ({ 
        ...prev, 
        [type]: { id, content } 
    }));

    const parseGeoJSON = (fileContent) => {
        try {
            return JSON.parse(fileContent);
        } catch (err) {
            console.error("Error parsing GeoJSON:", err);
            toast(t("geojson_parsing_error"), "error");
            return null;
        }
    };

    const handleClientFile = async (file, upload_id) => {
        // Read file content to display features and terrain elevation on map
        try {
            const content = await readFile(file);
            const format = getFileFormat(content);

            let result = null;
            if (format === ".geojson") {
                result = { 
                    type: "features", 
                    content: parseGeoJSON(content) 
                };
            } else if (format === ".csv") {
                result = { 
                    type: "elevation_map", 
                    content: csvToElevation(content) 
                };
            } else {
                toast(t("unsupported_file"), "error");
            }

            if (result?.content) {
                toast(`Archivo ${file.name} cargado exitosamente`, "success");
                setModelContent(result.type, upload_id, result.content);
            }
        } catch (err) {
            console.error("File read error:", err);
            toast(t("file_read_error"), "error");
        }
    };

    // Upload file to backend
    const uploadFile = useCallback( async file => {
        preloader(true);
        try {
            // Check if valid file format
            const extension = "." + file.name.split(".").pop().toLowerCase();
            if (!Object.keys(extensionToType).includes(extension)) {
                toast(t("unsupported_file"), "error");
                preloader(false);
                return;
            }

            // If there is an existing file of the same type, remove it first
            const existingId = model[extensionToType[extension]].id;
            if(existingId) {
                await removeFile(existingId, extension);
            }
            
            // Try to upload the file to the server
            // Response will be { upload_id, extension, data(only for .nc files) }
            const formData = new FormData();
            formData.append("file", file);
            const res = await fetchWithTimeout(api("/api/upload"), {
                method: "POST",
                body: formData,
            });

            let upload_id = null;
            let data = null;
            if (!res.ok) {
                toast("Servidor no disponible", "error");
            }else{ // If successful upload -> set upload_id (and content for .nc files)
                ({ upload_id, data } = await res.json());
                if (extension === ".nc") {
                    setModelContent("elevation_map", upload_id, data);
                    toast(`Archivo ${file.name} cargado exitosamente`, "success");
                    return; // No need to handle client file for .nc
                }
            }

            // Independent of upload success, try to read and parse the file on client side
            await handleClientFile(file, upload_id);
            
        } catch (err) {
            console.error("Upload error:", err);
            toast("Error al cargar el archivo", "error");
            throw err;
        } finally {
            preloader(false);
        }
    }, [toast, preloader]);

    // Remove file from state (doesn't delete from backend)
    const removeFile = useCallback( async (upload_id, format) => {
            const extension = format === ".geojson" ? ".json" : format;
            try {
                const res = await fetchWithTimeout(api("/api/delete"), {
                    method: "POST",
                    headers: {
                        "Content-Type": "application/json"
                    },
                    body: JSON.stringify({
                        upload_id,
                        extension
                    }),
                });
                if (!res.ok) {
                    toast(t("cannot_delete_file"), "error");
                    //throw new Error("Failed to delete file from server");
                }

                if ([".csv", ".nc"].includes(format)) {
                    setModelContent("elevation_map", null, null);
                } else if (format === ".geojson") {
                    setModelContent("features", null, null);
                } else {
                    toast(t("unsupported_file"), "error");
                    return;
                }
                toast(t("file_deletion_success"), "success");
            } catch (err) {
                console.error("Delete error:", err);
            }
        },
        [toast]
    );

    return { model, setModel, uploadFile, removeFile  };
}

export default useModel;