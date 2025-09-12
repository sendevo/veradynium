import { 
    LineChart as Chart, 
    Line, 
    XAxis, 
    YAxis, 
    CartesianGrid, 
    Tooltip, 
    ResponsiveContainer 
} from "recharts";

const toolTipStyle = { backgroundColor: "#333", border: "1px solid #ccc" };

const LineChart = ({data}) => {

    if(!data || !Array.isArray(data)){
        console.warn("LineChart: Invalid or missing data prop");
        return null;
    }

    const values = data.map((v, i) => ({ index: i, altura: v }));

    return (
        <ResponsiveContainer width="100%" height={300}>
            <Chart data={values}>
                <CartesianGrid strokeDasharray="3 3" />
                
                <XAxis dataKey="index" />
                <YAxis />
                
                <Tooltip 
                    labelFormatter={() => ""}
                    contentStyle={toolTipStyle}
                    itemStyle={{ color: "white" }}
                    labelStyle={{ color: "white" }}/>

                <Line type="monotone" dataKey="altura" stroke="#ffffff" />
            </Chart>
        </ResponsiveContainer>
    );
};

export default LineChart;
