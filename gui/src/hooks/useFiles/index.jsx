import { useState, useCallback } from "react";
import { api } from '../../model/constants';
import { 
    fetchWithTimeout,
    getFileFormat,
    csvToElevation
} from "../../model/utils";

const initialFiles = {
  elevation_map: { id: null, content: null },
  features: { id: null, content: null },
};

const extensionToType = {
    ".csv": "elevation_map",
    ".nc": "elevation_map",
    ".geojson": "features",
    ".json": "features"
};


const useFiles = (toast, preloader) => {
    
    const [files, setFiles] = useState(initialFiles);

    const setFile = (type, id, content) => setFiles((prev) => ({ ...prev, [type]: { id, content } }));

    const parseGeoJSON = (fileContent) => {
        try {
            return JSON.parse(fileContent);
        } catch (err) {
            console.error("Error parsing GeoJSON:", err);
            toast("Error al parsear GeoJSON", "error");
            return null;
        }
    };

    const handleClientFile = (file, upload_id) => {
        return new Promise((resolve) => {
            const reader = new FileReader();
            reader.onload = () => {
                const content = reader.result;
                const format = getFileFormat(content);
                if (format === ".geojson") {
                    resolve({
                        type: "features",
                        content: parseGeoJSON(content)
                    });
                } else if (format === ".csv") {
                    resolve({
                        type: "elevation_map",
                        content: csvToElevation(content)
                    });
                } else {
                    toast("Formato de archivo no soportado", "error");
                    resolve(null);
                }
            };
            reader.readAsText(file);
        })
        .then((result) => {
            if (result ?.content) {
                toast(`Archivo ${file.name} cargado exitosamente`, "success");
                setFile(result.type, upload_id, result.content);
            }
        });
    };

    // Upload file to backend
    const uploadFile = useCallback( async file => {
            preloader(true);
            try {
                const extension = "." + file.name.split(".").pop().toLowerCase();
                if (!Object.keys(extensionToType).includes(extension)) {
                    toast("Formato de archivo no soportado", "error");
                    preloader(false);
                    return;
                }

                const existingId = files[extensionToType[extension]].id;
                if(existingId) {
                    await removeFile(existingId, extension);
                }
                
                const formData = new FormData();
                formData.append("file", file);
                
                const res = await fetchWithTimeout(api("/api/upload"), {
                    method: "POST",
                    body: formData,
                });

                if (!res.ok) {
                    toast("Error al cargar el archivo", "error");
                    throw new Error("Failed to upload file");
                }

                const { upload_id, data } = await res.json();

                if (extension === ".nc") {
                    setFile("elevation_map", upload_id, data);
                    toast(`Archivo ${file.name} cargado exitosamente`, "success");
                } else {
                    await handleClientFile(file, upload_id);
                }
            } catch (err) {
                console.error("Upload error:", err);
                toast("Error al cargar el archivo", "error");
                throw err;
            } finally {
                preloader(false);
            }
        }, [toast, preloader]
    );

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
                if (!res.ok) throw new Error("Failed to delete file from server");

                if ([".csv", ".nc"].includes(format)) {
                    setFile("elevation_map", null, null);
                } else if (format === ".geojson") {
                    setFile("features", null, null);
                } else {
                    toast("Formato de archivo no soportado", "error");
                    return;
                }
                toast("Archivo eliminado exitosamente", "success");
            } catch (err) {
                console.error("Delete error:", err);
            }
        },
        [toast]
    );

    return { files, uploadFile, removeFile  };
}

export default useFiles;