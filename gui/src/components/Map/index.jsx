import { useState } from 'react';
import { Box } from '@mui/material';
import { 
    MapContainer, 
    TileLayer, 
    FeatureGroup,
    GeoJSON
} from 'react-leaflet';
import ZoomWatcher from './zoomWatcher';
import HeatMapLayer from './heatmapLayer';
import { pointToLayer } from './icons';


const Map = ({ 
    mapCenter, 
    initialZoom = 13, 
    featureCollection = null, 
    elevationData = []}) => {
    // featureCollection is expected to be a valid GeoJSON FeatureCollection
    // elevationData is expected to be an array of [lat, lng, intensity]
    // Example: [[-45.86, -67.51, 0.5], [-45.87, -67.52, 0.8], ...]

    const [zoom, setZoom] = useState(initialZoom);

    return (
        <Box sx={{ height: "100%", width: "100%", m: 0 }}>
            <MapContainer 
                center={mapCenter} 
                zoom={zoom} 
                style={{ height: "100%", width: "100%" }}>
                    <TileLayer url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"/>
                    {featureCollection.features.length > 0 &&
                        <FeatureGroup>
                            <GeoJSON data={featureCollection} pointToLayer={pointToLayer}/>
                        </FeatureGroup>
                    }
                    {elevationData.length > 0 &&
                        <HeatMapLayer
                            zoom={zoom}
                            points={elevationData} // [[lat, lng, intensity], ...]
                        />
                    }
                    <ZoomWatcher setZoom={setZoom}/>
            </MapContainer>
        </Box>
    );
};

export default Map;