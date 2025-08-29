import { Link, useLocation } from 'react-router-dom';
import { AppBar, Box, Toolbar, Button } from "@mui/material";
import { removeSlash } from '../../model/utils';
import views from "../../views";

const toolbarStyle = {
    backgroundColor: "#333333 !important",
    opacity: "0.8"
};

const Navigation = () => {
    
    const location = useLocation();

    const path = location.pathname.split('/')[1] || 'home';

    return (
        <Box sx={{ flexGrow: 1}} >
            <AppBar position="static" color="primary" sx={toolbarStyle}>
                <Toolbar>
                    {
                        views.filter(v => Boolean(v.name)).map((v,k) => (
                            <Button key={k} LinkComponent={Link} to={v.path} color="inherit">
                                <span style={{fontWeight: path==removeSlash(v.path) ? "bold" : "normal"}}>{v.name}</span>
                            </Button>
                        ))
                    }
                </Toolbar>
            </AppBar>
        </Box>
    );
};


export default Navigation;