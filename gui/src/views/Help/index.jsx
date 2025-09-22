import MainView from "../../components/MainView";
import { useTranslation } from "react-i18next";

const View = () => {
    const { t } = useTranslation("help");

    return (
        <MainView title={t("title")}>
            {t("title")}
        </MainView>
    );
};

export default View;