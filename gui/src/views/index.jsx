import Home from "./Home";
import Data from "./Data";
import About from "./About";
//import Help from "./Help";
import Error from "./Error";

const views = [
    {
        path: "/home",
        name: "home",
        component: <Home />
    },
    {
        path: "/data",
        name: "details",
        component: <Data />
    },
    {
        path: "/about",
        name: "about",
        component: <About />
    },
    /*{
        path: "/help",
        name: "help",
        component: <Help />
    },*/
    {
        path: "/error",
        component: <Error />
    }
];

export default views;