import { 
    BarChart, 
    Bar, 
    XAxis, 
    YAxis, 
    Tooltip, 
    CartesianGrid,
    ResponsiveContainer
} from "recharts";
import { useTranslation } from "react-i18next";
import "./tooltipStyle.css";

const toolTipStyle = { 
    backgroundColor: "#333", 
    border: "1px solid #ccc",
    boxShadow: "none",
    color: "#fff" 
};

const Histogram = ({ binSize, values }) => {
    if (!Array.isArray(values) || typeof binSize !== "number") return null;

    const { t } = useTranslation("charts");

    // Convert histogram array to data points
    const data = values.map((count, i) => ({
        range: `${(i * binSize)}–${((i + 1) * binSize)} m`,
        count,
    }));

    return (
        <ResponsiveContainer width="100%" height={300}>
            <BarChart
                width={500}
                height={300}
                data={data}
                margin={{ top: 10, right: 20, bottom: 20, left: 20 }}>
                <CartesianGrid strokeDasharray="3 3" />
                <XAxis dataKey="range" angle={-45} textAnchor="end" height={60} />
                <YAxis />
                <Tooltip 
                    contentStyle={toolTipStyle}
                    formatter={(value, name) => {
                        if(name === "count") return [value, t("count")];
                        if(name === "range") return [value, t("range")];
                        return [value, name];
                    }}/>
                <Bar 
                    dataKey="count" 
                    fill="#3182CE" />
            </BarChart>
        </ResponsiveContainer>
    );
};

export default Histogram;