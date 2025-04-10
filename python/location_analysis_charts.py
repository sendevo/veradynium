#from location_analysis import export_analysis_data
import pandas as pd
import matplotlib.pyplot as plt
import io
from location_analysis import export_analysis_data


device_id = "hyzim-u-h2-7024106395"
csv_data = export_analysis_data(device_id)

df = pd.read_csv(io.StringIO(csv_data))

df.plot(x='distance', y='airtime', kind='scatter', grid=True)

plt.title(f"Distance vs Airtime for Device {device_id}")
plt.xlabel("Distance (km)")
plt.ylabel("Airtime (seconds)")
plt.savefig("distance_vs_airtime.png")
plt.show()
