import { useState, useCallback } from "react";
import { api } from '../../model/constants';
import { fetchWithTimeout } from "../../model/utils";


const useFileIds = () => {
    // State to hold uploaded files metadata
    // em_file: elevation map file id (csv extension)
    // features_file: geojson features file id (.json extension used in server side)
    
    const [fileIds, setFileIds] = useState({
        em_file: null, // id of uploaded elevation map file
        features_file: null // id of uploaded geojson features file
    });

    // Upload file to backend
    const uploadFile = useCallback(async file => {
        try{
            const formData = new FormData();
            formData.append("file", file);
            const res = await fetchWithTimeout(api("/api/upload"), {
                method: "POST",
                body: formData,
            });
            if (!res.ok) 
                throw new Error("Failed to upload file");

            // Server will extract file extension and return unique id
            const data = await res.json(); // { upload_id: "unique-file-id"+(".csv" | ".json") }
            
            setFileIds(prev => {
                const nextFileIds = { ...prev };

                switch (data.extension) {
                    case ".csv":
                        nextFileIds.em_file = data.upload_id;
                        break;
                    case ".json":
                        nextFileIds.features_file = data.upload_id;
                        break;
                    default:
                        throw new Error("Unsupported file type");
                }

                return nextFileIds;
            });
        } catch(err) {
            console.error("Upload error:", err);
            throw err;
        }
    }, []);

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
        }catch(err){
            console.error("Delete error:", err);
        }

        setFileIds(prev => {
            const nextFileIds = { ...prev };
            if (extension === ".csv") 
                nextFileIds.em_file = null;
            if (extension === ".geojson") 
                nextFileIds.features_file = null;
            return nextFileIds;
        });
    }, [fileIds, setFileIds]);

    return { fileIds, uploadFile, removeFile  };
}

export default useFileIds;