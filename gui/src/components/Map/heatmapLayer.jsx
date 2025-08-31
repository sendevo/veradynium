import { useEffect } from 'react';
import L from 'leaflet';
import 'leaflet.heat';
import { useMap } from 'react-leaflet';


const radiusByZoom = [1, 1, 1, 2, 2, 3, 4, 5, 7, 8, 10, 12, 13, 15, 20, 25, 30, 38, 45];

const HeatMapLayer = ({points, zoom}) => {
    // points is expected to be an array of [lat, lng, intensity]
    // zoom is the current map zoom level
    // Example: [[-45.86, -67.51, 0.5], [-45.87, -67.52, 0.8], ...]
    
    const map = useMap();

    const radius = radiusByZoom[Math.min(zoom, radiusByZoom.length - 1)];

    useEffect(() => {    
        if(points.length === 0) 
            return;
        const heatLayer = L.heatLayer(points, {radius}).addTo(map);
        return () => { map.removeLayer(heatLayer); };
    }, [points, zoom]);

    return null;
};

export default HeatMapLayer;