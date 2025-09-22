import { Button, Grid } from '@mui/material';

const MenuButtons = ({
    hasFeatures,
    hasElevation,
    hasUploadedFiles,
    handleComputeLOS,
    evalNetworkAction,
    runSolverAction,
    handleRemoveFeatures,
    handleRemoveElevation,
    handleResetPoints,
    points
}) => (
    <>
        {hasFeatures && hasElevation && hasUploadedFiles &&
            <Grid>
                <Button 
                    fullWidth
                    onClick={evalNetworkAction}
                    variant="contained">
                        Test conectividad
                </Button>
            </Grid>
        }

        {hasFeatures && hasElevation && hasUploadedFiles &&
            <Grid>
                <Button 
                    fullWidth
                    onClick={runSolverAction}
                    variant="contained">
                        Ejecutar solver
                </Button>
            </Grid>
        }

        {points.length === 2 && hasElevation &&
            <>
                {hasUploadedFiles &&
                    <Grid>
                        <Button 
                            fullWidth
                            onClick={handleComputeLOS}
                            variant="contained">
                                Evaluar LOS
                        </Button>
                    </Grid>
                }
                <Grid>
                    <Button 
                        fullWidth
                        color="secondary"
                        onClick={handleResetPoints}
                        variant="contained">
                            Restablecer puntos
                    </Button>
                </Grid>
            </>
        }
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
    </>
);

export default MenuButtons;