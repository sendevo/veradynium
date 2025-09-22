import { use, useState } from 'react';
import { Link, useLocation } from 'react-router-dom';
import { AppBar, Box, Toolbar, Button } from "@mui/material";
import i18n from "i18next";
import { useTranslation } from "react-i18next";
import { removeSlash } from '../../model/utils';
import views from "../../views";
import useToast from '../../hooks/useToast';
import argFlag from "../../assets/icons/argentina_flag.png";
import engFlag from "../../assets/icons/uk_flag.png";


const toolbarStyle = {
    backgroundColor: "#333333 !important",
    opacity: "0.8"
};

const icons = {
    es: argFlag,
    en: engFlag
};


const Navigation = () => {
    
    const location = useLocation();
    const path = location.pathname.split('/')[1] || 'home';
    const [ locale, setLocale ] = useState(i18n.language || 'es');
    const { t } = useTranslation("navigation");
    const toast = useToast();

    const switchLocale = () => {
        const newLocale = locale === 'es' ? 'en' : 'es';
        i18n.changeLanguage(newLocale);
        toast(t("switch_language"), 'info');
        setLocale(newLocale);
    };

    return (
        <Box sx={{ flexGrow: 1}} >
            <AppBar position="static" color="primary" sx={toolbarStyle}>
                <Toolbar>
                    {
                        views.filter(v => Boolean(v.name)).map((v,k) => (
                            <Button key={k} LinkComponent={Link} to={v.path} color="inherit">
                                <span style={{fontWeight: path==removeSlash(v.path) ? "bold" : "normal"}}>{t(v.name)}</span>
                            </Button>
                        ))
                    }
                    <Box sx={{ flexGrow: 1 }} />
                    <Button onClick={switchLocale} color="inherit">
                        <img src={icons[locale]} alt={t("alt")} style={{width: '40px'}} />
                    </Button>
                </Toolbar>
            </AppBar>
        </Box>
    );
};


export default Navigation;