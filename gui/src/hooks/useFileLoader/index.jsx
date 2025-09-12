import { useState } from "react";
import { 
    csvToArray, 
    chunkedMax, 
    chunkedMin, 
    normalizeElevation 
} from "../../model/utils";
import { useFileIdsContext } from "../../context/FileIds";


const useFileLoader = toast => {
    const [featureCollection, setFeatureCollection] = useState({ features: [] });
    const [elevationData, setElevationData] = useState([]);
    const { fileIds, removeFile } = useFileIdsContext();

    const loadFile = (data, extension) => {
        switch (extension) {
            case ".geojson":
                try {
                    const fc = JSON.parse(data);
                    setFeatureCollection(fc);
                    toast(`GeoJSON cargado. Geometrías totales: ${fc.features.length}`, "success");
                } catch (err) {
                    toast("Error al parsear GeoJSON", "error");
                }
                break;
            case ".csv":
                try {
                    const arr = csvToArray(data, { withHeaders: false, delimiter: ",", maxRows: -1 });
                    const alts = arr.map(p => p[2]);
                    const normalized = normalizeElevation(arr, chunkedMin(alts), chunkedMax(alts));
                    setElevationData(normalized);
                    toast(`Datos de elevación cargados. Total de puntos: ${arr.length}`, "success");
                } catch (err) {
                    toast("Error al parsear CSV", "error");
                }
                break;
            default:
                toast("Formato de archivo no soportado", "error");
        }
    };

    const removeElements = (type) => {
        switch (type) {
            case "features":
                if (fileIds.features_file) removeFile(fileIds.features_file, ".geojson");
                setFeatureCollection({ features: [] });
                break;
            case "elevation":
                if (fileIds.em_file) removeFile(fileIds.em_file, ".csv");
                setElevationData([]);
                break;
            default:
                console.warn("Unknown type to remove:", type);
        }
    };

    return {
        fileIds,
        featureCollection,
        elevationData,
        loadFile,
        removeElements,
    };
};

export default useFileLoader;
