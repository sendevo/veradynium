import { Button, Grid } from '@mui/material';

const MenuButtons = ({
    hasFeatures,
    hasElevation,
    points,
    handleRemoveFeatures,
    handleRemoveElevation,
    handleResetPoints,
    handleEvalNetwork,
    handleComputeLOS,
    handleRunSolver
}) => (
    <>
        {hasFeatures > 0 &&
            <Grid>
                <Button 
                    fullWidth
                    color="secondary"
                    onClick={handleRemoveFeatures}
                    variant="contained">
                        Quitar geometrías
                </Button>
            </Grid>
        }

        {hasElevation > 0 &&
            <Grid>
                <Button 
                    fullWidth
                    color="secondary"
                    onClick={handleRemoveElevation}
                    variant="contained">
                        Quitar altimetría
                </Button>
            </Grid>
        }

        {points.length === 2 && hasElevation &&
            <>
                <Grid>
                    <Button 
                        fullWidth
                        color="secondary"
                        onClick={handleResetPoints}
                        variant="contained">
                            Restablecer puntos
                    </Button>
                </Grid>
                <Grid>
                    <Button 
                        fullWidth
                        onClick={handleComputeLOS}
                        variant="contained">
                            Evaluar LOS
                    </Button>
                </Grid>
            </>
        }

        {hasFeatures && hasElevation && 
            <Grid>
                <Button 
                    fullWidth
                    onClick={handleEvalNetwork}
                    variant="contained">
                        Test conectividad
                </Button>
            </Grid>
        }

        {hasFeatures && hasElevation &&
            <Grid>
                <Button 
                    fullWidth
                    onClick={handleRunSolver}
                    variant="contained">
                        Ejecutar solver
                </Button>
            </Grid>
        }
    </>
);

export default MenuButtons;