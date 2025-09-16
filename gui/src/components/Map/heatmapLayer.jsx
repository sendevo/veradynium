import { useEffect, useRef } from 'react';
import L from 'leaflet';
import 'leaflet.heat';
import { useMap } from 'react-leaflet';

const radiusByZoom = [1, 1, 1, 2, 2, 3, 4, 5, 7, 8, 10, 12, 13, 15, 20, 25, 30, 38, 45];

const HeatMapLayer = ({ points, zoom }) => {
  const map = useMap();
  const layerRef = useRef(null);

  useEffect(() => {
    if (!layerRef.current) {
      // create heat layer only once
      layerRef.current = L.heatLayer(points, { radius: radiusByZoom[Math.min(zoom, radiusByZoom.length - 1)] }).addTo(map);
    } else {
      // update points
      layerRef.current.setLatLngs(points);
      // update radius if zoom changed
      layerRef.current.setOptions({ radius: radiusByZoom[Math.min(zoom, radiusByZoom.length - 1)] });
    }

    // cleanup on unmount
    return () => {
      if (layerRef.current) {
        map.removeLayer(layerRef.current);
        layerRef.current = null;
      }
    };
  }, [points, zoom, map]);

  return null;
};

export default HeatMapLayer;
