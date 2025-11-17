import { Typography, Box } from "@mui/material";
import { useTranslation } from "react-i18next";

const tableContainerStyle = {
    maxHeight: "50vh",
    overflow: "auto",
    color: "white"
};

const tableStyle = {
    color:"white",
    width: "100%",
    borderCollapse: "collapse",
    border: "1px solid white",
    textAlign: "left",
    backgroundColor: 'rgba(0,0,0,0.6)',
    fontSize: 16,
    overflowX: "auto"
};

const highlightedRowStyle = {
    backgroundColor: "rgba(255,155,155,0.5)"
};

const Cell = ({content}) => (
    <td style={{border: "1px solid white", padding: 8}}>
        <Typography>
            {content}
        </Typography>
    </td>
);

const getCoordinate = (feature, coord) => feature.geometry.coordinates[ coord ==="lat"?1:0].toFixed(5)

export const EDTable = ({ featureCollection }) => {

    const { t } = useTranslation("nodes_table");

    const nodes = featureCollection.features.filter(feature => feature.geometry.type === "Point" && feature.properties.type === "end_device");
    const hasFeatures = nodes.length > 0;

    return (
        hasFeatures &&
            <Box>
                <Typography sx={{fontWeight:"bold", mb:2, mt:2}}>{t("end_devices")}:</Typography>
                <Box style={tableContainerStyle}>
                    <table style={tableStyle}>
                        <thead>
                            <tr>
                                <th>{t("id")}</th>
                                <th>{t("position")}</th>
                                <th>{t("gateway")}</th>
                                <th>{t("sf")}</th>
                            </tr>
                        </thead>
                        <tbody>
                            {
                                nodes.map((feature, index) => (
                                    <tr key={index} style={feature.properties.selected ? highlightedRowStyle : {}}>
                                        <Cell content={feature.properties.id}/>
                                        <Cell content={`(${getCoordinate(feature, "lat")}, ${getCoordinate(feature, "lng")}, ${feature.properties.height}m)`}/>
                                        <Cell content={
                                            feature.properties.assigned_gateway ? feature.properties.assigned_gateway : "-"
                                        }/>
                                        <Cell content={feature.properties.sf ? feature.properties.sf : "-"}/>
                                    </tr>
                                )
                            )}
                        </tbody>
                    </table>
                </Box>
            </Box>
    );
};

export const GWTable = ({ featureCollection }) => {
    const { t } = useTranslation("nodes_table");

    const gateways = featureCollection.features.filter(feature => feature.geometry.type === "Point" && feature.properties.type === "gateway");
    const hasFeatures = gateways.length > 0;

    return (
        hasFeatures && 
            <Box>
                <Typography sx={{fontWeight:"bold", mb:2, mt:2}}>{t("gateways")}:</Typography>
                <Box style={{...tableContainerStyle}}>
                    <table style={tableStyle}>
                        <thead>
                            <tr>
                                <th>{t("id")}</th>
                                <th>{t("position")}</th>
                                <th>{t("end_devices")}</th>
                            </tr>
                        </thead>
                        <tbody>
                            {
                                gateways.map((feature, index) => (
                                    <tr key={index} style={feature.properties.selected ? highlightedRowStyle : {}}>
                                        <Cell content={feature.properties.id}/>
                                        <Cell content={`(${getCoordinate(feature, "lat")}, ${getCoordinate(feature, "lng")}, ${feature.properties.height}m)`}/>
                                        <Cell content={
                                            feature.properties.type === "end_device" ? 
                                            (feature.properties.assigned_gateway ? feature.properties.assigned_gateway : "-")
                                            :
                                            (feature.properties.connected_devices ? feature.properties.connected_devices.length : "0")
                                        }/>
                                    </tr>
                                )
                            )}
                        </tbody>
                    </table>
                </Box>
            </Box>
    );
}