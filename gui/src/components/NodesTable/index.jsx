import { Typography, Box } from "@mui/material";

const tableContainerStyle = {
    maxHeight: "65vh",
    overflowY: "auto",
    color: "white"
};

const tableStyle = {
    color:"white",
    width: "100%",
    borderCollapse: "collapse",
    border: "1px solid white",
    textAlign: "left",
    backgroundColor: 'rgba(0,0,0,0.6)',
    fontSize: 16
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

const NodesTable = ({ featureCollection }) => {

    const hasFeatures = Array.isArray(featureCollection.features) && featureCollection.features.length > 0;

    return (
        <Box style={tableContainerStyle}>
            {hasFeatures ?
                <table style={tableStyle}>
                    <thead>
                        <tr>
                            <th>Tipo</th>
                            <th>Id</th>
                            <th>Pos.</th>
                        </tr>
                    </thead>
                    <tbody>
                        {featureCollection.features
                            .filter(feature => feature.geometry.type === "Point")
                            .map((feature, index) => (
                                <tr key={index} style={feature.properties.selected ? highlightedRowStyle : {}}>
                                    <Cell content={feature.properties.type === "end_device" ? "Disp. final" : "Gateway"}/>
                                    <Cell content={feature.properties.id}/>
                                    <Cell content={
                                        `(${getCoordinate(feature, "lat")},
                                        ${getCoordinate(feature, "lng")})`
                                        }/>
                                </tr>
                            )
                        )}
                    </tbody>
                </table>
                :
                <Box sx={{height:"65vh", display:"flex", justifyContent:"center", alignItems:"center"}}>
                    <Typography variant="h6" align="center" color="white" sx={{mt:2}}>
                        No hay nodos para mostrar. <br/> Cargue un archivo de puntos o agregue nodos en el mapa.
                    </Typography>
                </Box>
            }
        </Box>
    );
};

export default NodesTable;