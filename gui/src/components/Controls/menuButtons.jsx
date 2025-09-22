import { Button, Grid } from '@mui/material';
import { useTranslation } from "react-i18next";

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
}) => {

    const { t } = useTranslation("controls");

    return (
        <>
            {hasFeatures && hasElevation && hasUploadedFiles && typeof evalNetworkAction === "function" &&
                <Grid>
                    <Button 
                        fullWidth
                        onClick={evalNetworkAction}
                        variant="contained">
                            {t("connectivity_test")}
                    </Button>
                </Grid>
            }

            {hasFeatures && hasElevation && hasUploadedFiles && typeof runSolverAction === "function" &&
                <Grid>
                    <Button 
                        fullWidth
                        onClick={runSolverAction}
                        variant="contained">
                            {t("run_solver")}
                    </Button>
                </Grid>
            }

            {points?.length === 2 &&
                <>
                    {hasUploadedFiles && typeof handleComputeLOS === "function" &&
                        <Grid>
                            <Button 
                                fullWidth
                                onClick={handleComputeLOS}
                                variant="contained">
                                    {t("compute_los")}
                            </Button>
                        </Grid>
                    }
                    { typeof handleResetPoints === "function" &&
                        <Grid>
                            <Button 
                                fullWidth
                                color="secondary"
                                onClick={handleResetPoints}
                                variant="contained">
                                    {t("reset_points")}
                            </Button>
                        </Grid>
                    }
                </>
            }
            {hasFeatures > 0 && typeof handleRemoveFeatures === "function" &&
                <Grid>
                    <Button 
                        fullWidth
                        color="secondary"
                        onClick={handleRemoveFeatures}
                        variant="contained">
                            {t("remove_features")}
                    </Button>
                </Grid>
            }

            {hasElevation > 0 && typeof handleRemoveElevation === "function" &&
                <Grid>
                    <Button 
                        fullWidth
                        color="secondary"
                        onClick={handleRemoveElevation}
                        variant="contained">
                            {t("remove_elevation")}
                    </Button>
                </Grid>
            }
        </>
    );
};

export default MenuButtons;