import { 
    LineChart as Chart, 
    Line, 
    XAxis, 
    YAxis, 
    CartesianGrid, 
    Tooltip, 
    ResponsiveContainer
} from "recharts";
import { useTranslation } from "react-i18next";
import { US915_LORA_LAMBDA, FRESNEL_ZONE_CLEARANCE } from "../../model/constants";

const toolTipStyle = { backgroundColor: "#333", border: "1px solid #ccc" };

const LineChart = ({elev_data, dist_data}) => {
    if(!Array.isArray(elev_data) || !Array.isArray(dist_data)){
        console.warn("LineChart: Invalid or missing elev_data or dist_data prop");
        return null;
    }

    const { t } = useTranslation("charts");
    
    const firstPoint = { distance: dist_data[0], altura: elev_data[0] };
    const lastPoint = { distance: dist_data[dist_data.length - 1], altura: elev_data[elev_data.length - 1] };
  
    const values = elev_data.map((_, i) => {
        const progress = (dist_data[i] - firstPoint.distance) / (lastPoint.distance - firstPoint.distance);
        const referenceValue = firstPoint.altura + progress * (lastPoint.altura - firstPoint.altura);

        // distances from point to each end
        const d1 = dist_data[i] - firstPoint.distance;
        const d2 = lastPoint.distance - dist_data[i];

        // first Fresnel radius
        const r1 = Math.sqrt((US915_LORA_LAMBDA * d1 * d2) / (d1 + d2));

        const clearance = FRESNEL_ZONE_CLEARANCE * r1;

        return {
            distance: dist_data[i],
            elev: elev_data[i],
            reference: referenceValue,
            fresnelUpper: referenceValue + clearance,
            fresnelLower: referenceValue - clearance
        };
    });

    return (
        <ResponsiveContainer width="100%" height={300}>
            <Chart data={values}>
                
                <CartesianGrid strokeDasharray="3 3" />
                
                <XAxis dataKey="distance" />
                <YAxis />
                
                <Tooltip 
                    labelFormatter={d => `${t("distance")}: ${d} m`} 
                    formatter={(value, name) => {
                        if(name === "elev") return [`${value.toFixed(2)} m`, t("elevation")];
                        if(name === "reference") return [`${value.toFixed(2)} m`, t("reference")];
                        if(name === "fresnelUpper") return [`${value.toFixed(2)} m`, t("fresnelUpper")];
                        if(name === "fresnelLower") return [`${value.toFixed(2)} m`, t("fresnelLower")];
                        return [value, name];
                    }}
                    contentStyle={toolTipStyle} 
                    itemStyle={{ color: "white" }} 
                    labelStyle={{ color: "white" }}/>
                
                <Line type="monotone" dataKey="elev" stroke="#ffffff" />

                <Line 
                    type="linear" 
                    dataKey="reference" 
                    stroke="#ff0000" 
                    strokeWidth={2}
                    dot={false}/>

                <Line 
                    type="linear" 
                    dataKey="fresnelUpper" 
                    stroke="#00ff00" 
                    strokeDasharray="5 5"
                    dot={false}/>

                <Line 
                    type="linear" 
                    dataKey="fresnelLower" 
                    stroke="#00ff00" 
                    strokeDasharray="5 5"
                    dot={false}/>
            </Chart>
        </ResponsiveContainer>
    );
};

export default LineChart;
