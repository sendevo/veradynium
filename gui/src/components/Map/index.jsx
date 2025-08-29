import { useEffect } from 'react';
import L from "leaflet";
import { Box } from '@mui/material';
import "leaflet.heat";
import 'leaflet-draw';
import { 
    MapContainer, 
    TileLayer, 
    FeatureGroup,
    GeoJSON,
    useMap
} from 'react-leaflet';
import edIconURL from '../../assets/icons/ed-icon.png';
import gwIconURL from '../../assets/icons/gw-icon.png';
import defaultIconURL from '../../assets/icons/default-icon.png';

// Fix Leaflet's default icon issue in Vite/React
delete L.Icon.Default.prototype._getIconUrl;
L.Icon.Default.mergeOptions({
    iconRetinaUrl: new URL("leaflet/dist/images/marker-icon-2x.png", import.meta.url).toString(),
    iconUrl: new URL("leaflet/dist/images/marker-icon.png", import.meta.url).toString(),
    shadowUrl: new URL("leaflet/dist/images/marker-shadow.png", import.meta.url).toString(),
});

const icons = {
    gateway: L.icon({
        iconUrl: gwIconURL,
        iconSize: [15, 15],
        iconAnchor: [7.5, 7.5]
    }),
    end_device: L.icon({
        iconUrl: edIconURL,
        iconSize: [15, 15],
        iconAnchor: [7.5, 7.5]
    }),
    default: L.icon({
        iconUrl: defaultIconURL,
        iconSize: [15, 15],
        iconAnchor: [7.5, 7.5]
    })
};


const pointToLayer = (feature, latlng) => {
    const iconType = feature.properties.type; // Adjust this property name based on your GeoJSON structure
    const icon = icons[iconType] || icons.default; // Fallback to a default icon if needed
    return L.marker(latlng, { icon });
};


const HeatMapLayer = ({points}) => {
    const map = useMap();
    
    useEffect(() => {
        if(points.length === 0) return;
        const heatData = points.map(p => [p[0], p[1], p[2]/1000]); // Adding intensity value
        const heatLayer = L.heatLayer(heatData, {radius: 25}).addTo(map);
        return () => {
            map.removeLayer(heatLayer);
        };
    }, [points, map]);

    return null;
};


const Map = ({ mapCenter, featureCollection = null, elevationData = []}) => {

    const hasFeatures = featureCollection.features.length > 0;

    useEffect(() => {
        delete L.Icon.Default.prototype._getIconUrl;
    }, []);

    return (
        <Box sx={{ height: "100%", width: "100%", m: 0 }}>
            <MapContainer 
                center={mapCenter} 
                zoom={13} 
                style={{ height: "100%", width: "100%" }}>
                    <TileLayer url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"/>
                    {hasFeatures &&
                        <FeatureGroup key={JSON.stringify(featureCollection)}>
                            <GeoJSON data={featureCollection} pointToLayer={pointToLayer}/>
                            {elevationData.length > 0 &&
                                <HeatMapLayer points={elevationData} />
                            }
                        </FeatureGroup>
                    }
            </MapContainer>
        </Box>
    );
}

export default Map;