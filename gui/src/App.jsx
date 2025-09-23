import { BrowserRouter, Routes, Route, Navigate } from 'react-router-dom';
import i18next from 'i18next';
import { initReactI18next } from 'react-i18next';
import { CssBaseline, GlobalStyles } from "@mui/material";
import { ThemeProvider } from "@mui/material/styles"; 
import theme, { globalStyles } from "./themes";
import views from "./views";
import Home from "./views/Home";
import ErrorBoundary from './components/ErrorBoundary';
import Navigation from "./components/Navigation";
import { ModelProvider } from './context/Model';
import UIUtilsProvider from './context/UIFeedback';
import dictionary from './model/dictionary';

i18next.use(initReactI18next).init({
    resources: dictionary,
    lng: 'es',
    fallbackLng: 'es',
    interpolation: { escapeValue: false }
});

const App = () =>(
    <ThemeProvider theme={theme}>
        <CssBaseline />
        <GlobalStyles styles={globalStyles}/>
        <UIUtilsProvider>
            <ModelProvider>
                <BrowserRouter>
                    <ErrorBoundary>
                        <Navigation/>
                        <Routes>
                            <Route index element={<Home/>} />
                            {
                                views.map((v,k) => (
                                    <Route key={k} path={v.path} element={v.component} />
                                ))        
                            }
                            <Route path="*" element={<Navigate replace to="/" />} />
                        </Routes>
                    </ErrorBoundary>
                </BrowserRouter>
            </ModelProvider>
        </UIUtilsProvider>
    </ThemeProvider>
);

export default App
