import { useEffect, useRef } from 'react';
import { useMap } from "react-leaflet";
import L from "leaflet";
import "leaflet.heat";

const HeatMapLayer = ({ points, radius = 15, blur = 15, maxZoom = 17, gradient }) => {
    const map = useMap();
    const heatLayerRef = useRef(null);
    const pointsRef = useRef(points);

    // Default gradient if none provided
    const defaultGradient = {
        0.0: 'blue',
        0.25: 'cyan',
        0.5: 'lime',
        0.75: 'yellow',
        1.0: 'red'
    };

    useEffect(() => {
        pointsRef.current = points;
    }, [points]);

    useEffect(() => {
        if (!pointsRef.current.length) return;

        // Remove existing layer if it exists
        if (heatLayerRef.current) {
            map.removeLayer(heatLayerRef.current);
        }

        // Convert points to heatmap format [lat, lng, intensity]
        const heatData = pointsRef.current.map(p => [p[0], p[1], p[2]]);
        
        // Create new heat layer with proper configuration
        heatLayerRef.current = L.heatLayer(heatData, {
            radius: radius,
            blur: blur,
            maxZoom: maxZoom,
            gradient: gradient || defaultGradient,
            minOpacity: 0.05
        }).addTo(map);

        // Update heat layer on zoom and move events to handle dynamic adjustments
        const updateHeatmap = () => {
            if (heatLayerRef.current) {
                const currentZoom = map.getZoom();
                // Adjust radius based on zoom level
                const dynamicRadius = radius * Math.pow(2, currentZoom - 13);
                
                heatLayerRef.current.setOptions({
                    radius: Math.max(5, Math.min(dynamicRadius, 50))
                });
                
                // Force redraw
                heatLayerRef.current.redraw();
            }
        };

        map.on('zoomend', updateHeatmap);
        map.on('moveend', updateHeatmap);

        return () => {
            if (heatLayerRef.current) {
                map.removeLayer(heatLayerRef.current);
            }
            map.off('zoomend', updateHeatmap);
            map.off('moveend', updateHeatmap);
        };
    }, [map, radius, blur, maxZoom, gradient]);

    return null;
};

export default HeatMapLayer;