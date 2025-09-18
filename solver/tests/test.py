import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("movs.csv")

# Plot static end-devices
eds = df[df['type'] == 'ed']
plt.scatter(eds['lng'], eds['lat'], c='gray', marker='o', label='end-devices')

# Plot gateway movements
gws = df[df['type'] == 'gw']
for gw_id, group in gws.groupby('id'):
    plt.plot(group['lng'], group['lat'], marker='x', label=f'gw {gw_id}')

plt.xlabel("Longitude")
plt.ylabel("Latitude")
plt.legend()
plt.show()
