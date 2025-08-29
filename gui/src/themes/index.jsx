import { createTheme } from "@mui/material/styles"; 

export const globalStyles = {
    a:{ textDecoration: "none", fontWeight: "bold" }
};

export const componentsStyles = {
    paper: {
        backgroundColor: 'rgb(245, 245, 245)',
        padding: '10px'
    },
    title: {fontSize: "16px", fontWeight: "bold"},
    hintText: {
        fontStyle: "italic",
        fontSize: "12px",
        padding: "0px",
        margin: "0px",
        lineHeight: "1em",
        color: "rgb(100,100,100)"
    },
    headerCell: {fontWeight: "bold",p: '2px 10px'},
    tableCell: {
        padding: '2px 10px',
        whiteSpace: "nowrap",
        overflow: "hidden",
        textOverflow: "ellipsis"
    }
};

const theme = createTheme({
    typography: {
        fontFamily: "Montserrat, Open Sans, sans-serif"
    },
    palette: {
        mode: "light",
        primary: { main: "#393939", contrastText: "#FFFFFF" },
        secondary: { main: "#8A0808", contrastText: "#FFFFFF" },
        red: {main: "#DD0000", contrastText: "#FFFFFF"},
        darkRed: {main:"#8A0808", contrastText: "#FFFFFF"},
        green: {main: "#007700", contrastText: "#FFFFFF"},
        darkGreen: {main: "#088A29", contrastText: "#FFFFFF"}
    }
});

export default theme;