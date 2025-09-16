import { useState, useRef, useEffect } from 'react';
import { Box } from '@mui/material';
import { 
    MapContainer, 
    TileLayer, 
    FeatureGroup,
    GeoJSON,
    Marker,
    Popup,
    useMapEvents
} from 'react-leaflet';
import ZoomWatcher from './zoomWatcher';
import HeatMapLayer from './heatmapLayer';
import { pointToLayer } from './icons';


const GeoJSONLayer = ({ data, pointToLayer }) => {
  const layerRef = useRef();

  useEffect(() => {
    if (layerRef.current) {
      layerRef.current.clearLayers();
      layerRef.current.addData(data);
    }
  }, [data]);

  return <GeoJSON ref={layerRef} data={data} pointToLayer={pointToLayer} />;
};

const PointSetter = ({ setPoints }) => {
    useMapEvents({
        click: e => {
            setPoints(prev => {
                const newPoint = [...prev];
                if (newPoint.length >= 2) 
                    newPoint.shift(); // Remove the oldest point if we already have 2
                newPoint.push({...e.latlng, height_m: 2.0}); // {lat: Number, lng: Number, height_m: Number}
                return newPoint;
            });
        }
    });
    return null;
};

const Map = props => {
    // featureCollection is expected to be a valid GeoJSON FeatureCollection
    // elevationData is expected to be an array of [lat, lng, intensity]
    // Example: [[-45.86, -67.51, 0.5], [-45.87, -67.52, 0.8], ...]
    const { 
        mapCenter, 
        initialZoom = 13, 
        featureCollection = null, 
        elevationData = [],
        points,
        setPoints
    } = props;

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
                            <GeoJSONLayer data={featureCollection} pointToLayer={pointToLayer} />
                        </FeatureGroup>
                    }

                    {elevationData.length > 0 &&
                        <HeatMapLayer
                            zoom={zoom}
                            points={elevationData} // [[lat, lng, intensity], ...]
                        />
                    }

                    <PointSetter setPoints={setPoints}/>
                    
                    {points.map((pos, idx) => (
                        <Marker key={idx} position={pos}>
                            <Popup>Punto {idx + 1}</Popup>
                        </Marker>
                    ))}
                        
                    <ZoomWatcher setZoom={setZoom}/>
            </MapContainer>
        </Box>
    );
};

export default Map;