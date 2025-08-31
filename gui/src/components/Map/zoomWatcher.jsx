import { useEffect } from 'react';
import { useMap } from 'react-leaflet';

const ZoomWatcher = ({setZoom}) => {
    const map = useMap();

    useEffect(() => {
        const onZoom = () => setZoom(map.getZoom());
        map.on("zoomend", onZoom);
        return () => {
            map.off("zoomend", onZoom);
        };
    }, [map]);

    return null;
}

export default ZoomWatcher;