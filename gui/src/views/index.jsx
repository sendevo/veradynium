import Home from "./Home";
import About from "./About";
import Help from "./Help";
import Error from "./Error";

const views = [
    {
        path: "/home",
        name: "Inicio",
        component: <Home />
    },
    {
        path: "/about",
        name: "Acerca de",
        component: <About />
    },
    {
        path: "/help",
        name: "Ayuda",
        component: <Help />
    },
    {
        path: "/error",
        component: <Error />
    }
];

export default views;