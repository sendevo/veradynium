import { useState, useCallback } from "react";
import { api } from '../../model/constants';


const useFileIds = () => {
    // State to hold uploaded files metadata
    // em_file: elevation map file id (csv extension)
    // geojson: geojson features file id (json extension)
    
    const [fileIds, setFileIds] = useState({
        em_file: null, // id of uploaded elevation map file
        geojson: null // id of uploaded geojson features file
    });

    // Upload file to backend
    const uploadFile = useCallback(async file => {
        const formData = new FormData();
        formData.append("file", file);

        const res = await fetch(api("/api/upload"), {
            method: "POST",
            body: formData,
        });

        if (!res.ok) 
            throw new Error("Failed to upload file");

        // Server will extract file extension and return unique id
        const data = await res.json(); // { upload_id: "unique-file-id", extension: ".csv" | ".json" }

        const nextState = {...fileIds};
        
        switch(data.extension) {
            case ".csv":
                nextState.em_file = data.upload_id;
                break;
            case ".json":
                nextState.geojson = data.upload_id;
                break;
            default:
                throw new Error("Unsupported file type");
        }

        setFileIds(nextState);

    }, []);

    // Remove file from state (doesn't delete from backend)
    const removeFile = useCallback((id) => {
        const nextState = {...fileIds};
        if (nextState.em_file === id) nextState.em_file = null;
        if (nextState.geojson === id) nextState.geojson = null;
        setFileIds(nextState);
    }, []);

    return { fileIds, uploadFile, removeFile  };
}

export default useFileIds;