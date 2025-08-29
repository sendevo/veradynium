import { Container, Box } from '@mui/material';
import classes from './style.module.css';

const Component = ({title, background, children}) => (
    <Box>
        <Box className={classes.Background} 
            sx={{background:`linear-gradient(rgba(0,0,0,0.5), rgba(0,0,0,0.85)), url(${background})`}}>
        </Box>
        <Container className={classes.Container}>
            {title && 
                <h3 className={classes.Title}>
                    {title}
                </h3>
            }
            {children}            
        </Container>
    </Box>
);


export default Component;