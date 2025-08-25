import srtm
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


lat_center=-45.86 
lon_center=-67.475
lat_span=0.21
lon_span=0.25
step = 0.001
file_path = "topography.csv"


# Load SRTM data and save to CSV
def get_srtm_data(file_path, lat_center, lon_center, lat_span, lon_span, step=0.001):
    lat_min = lat_center - lat_span/2
    lat_max = lat_center + lat_span/2
    lng_min = lon_center - lon_span/2
    lng_max = lon_center + lon_span/2

    elevation_data = srtm.get_data()
    lats = np.arange(lat_min, lat_max, step)
    lngs = np.arange(lng_min, lng_max, step)

    data = []
    for lat in lats:
        for lng in lngs:
            alt = elevation_data.get_elevation(lat, lng)
            data.append([lat, lng, alt])

    df = pd.DataFrame(data, columns=["lat", "lng", "alt"])
    df.to_csv(file_path, index=False)
    print("CSV file saved as", file_path)


# Load elevation data from CSV
def load_elevation_data(file_path):
    df = pd.read_csv(file_path)
    return df

# Plot 3D surface
def plot3d(lats, lngs, alts, save = False): # Plot 3D surface
    lat_grid, lng_grid = np.meshgrid(lats, lngs, indexing="ij")
    alt_grid = np.array(alts.reshape(len(lats), len(lngs)))

    fig = plt.figure(figsize=(10, 6))
    ax = fig.add_subplot(111, projection="3d")
    ax.plot_surface(lat_grid, lng_grid, alt_grid, cmap="terrain")

    ax.set_xlabel("Latitude")
    ax.set_ylabel("Longitude")
    ax.set_zlabel("Altitude (m)")

    if save:
        plt.savefig("topography_3d.png", dpi=300)
        print("Saved 3D plot as topography_3d.png")
    plt.show()

# Plot 2D contour
def plot2d(lats, lngs, alts, save = False): # Plot 2D contour
    lat_grid, lng_grid = np.meshgrid(lats, lngs, indexing="ij")
    alt_grid = np.array(alts.reshape(len(lats), len(lngs)))

    plt.figure(figsize=(10, 6))
    contour = plt.contourf(lng_grid, lat_grid, alt_grid, cmap="terrain")
    plt.colorbar(contour, label="Altitude (m)")
    plt.xlabel("Longitude")
    plt.ylabel("Latitude")
    plt.title("Topography Contour")

    if save:
        plt.savefig("topography_2d.png", dpi=300)
        print("Saved 2D plot as topography_2d.png")
    plt.show()

#get_srtm_data(file_path, lat_center, lon_center, lat_span, lon_span, step)
df = load_elevation_data(file_path)

lat_min = lat_center - lat_span/2
lat_max = lat_center + lat_span/2
lng_min = lon_center - lon_span/2
lng_max = lon_center + lon_span/2
lats = np.arange(lat_min, lat_max, step)
lngs = np.arange(lng_min, lng_max, step)
alts = df['alt'].values

plot2d(lats, lngs, alts, save=True)

