import { 
    LineChart as Chart, 
    Line, 
    XAxis, 
    YAxis, 
    CartesianGrid, 
    Tooltip, 
    ResponsiveContainer,
    ReferenceLine 
} from "recharts";

const toolTipStyle = { backgroundColor: "#333", border: "1px solid #ccc" };

const LineChart = ({elev_data, dist_data}) => {
    if(!Array.isArray(elev_data) || !Array.isArray(dist_data)){
        console.warn("LineChart: Invalid or missing elev_data or dist_data prop");
        return null;
    }
    
    const firstPoint = { distance: dist_data[0], altura: elev_data[0] };
    const lastPoint = { distance: dist_data[dist_data.length - 1], altura: elev_data[elev_data.length - 1] };
  
    // Include dist_data in your values array
    const values = elev_data.map((_, i) => {
        const progress = (dist_data[i] - firstPoint.distance) / (lastPoint.distance - firstPoint.distance);
        const referenceValue = firstPoint.altura + progress * (lastPoint.altura - firstPoint.altura);
        return {
            distance: dist_data[i],
            elev: elev_data[i],
            reference: referenceValue
        };
    });

    return (
        <ResponsiveContainer width="100%" height={300}>
            <Chart data={values}>
                
                <CartesianGrid strokeDasharray="3 3" />
                
                <XAxis dataKey="distance" />
                <YAxis />
                
                <Tooltip 
                    labelFormatter={() => ""} 
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
            </Chart>
        </ResponsiveContainer>
    );
};

export default LineChart;
