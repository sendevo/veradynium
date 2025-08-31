
import L from "leaflet";
import "leaflet.heat";
import 'leaflet-draw';
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


export const pointToLayer = (feature, latlng) => {
    const iconType = feature.properties.type;
    const icon = icons[iconType] || icons.default;
    return L.marker(latlng, { icon });
};