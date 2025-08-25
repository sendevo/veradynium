import xarray as xr
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import griddata

def nc_to_csv(nc_file, csv_file, lat_center=None, lon_center=None, lat_span=0.2, lon_span=0.2):
    """
    Read a NetCDF DEM file and save cropped or full data to CSV.
    
    Parameters:
        nc_file (str): Path to the .nc file.
        csv_file (str): Path to save the CSV output.
        lat_center, lon_center (float, optional): Center of bounding box. If None, saves full tile.
        box_size (float): Width/height of bounding box in degrees (default 0.2 ~ 20km).
    """
    ds = xr.open_dataset(nc_file)
    var_name = list(ds.data_vars.keys())[0]
    
    if lat_center is not None and lon_center is not None:
        lat_min = lat_center - lat_span/2
        lat_max = lat_center + lat_span/2
        lon_min = lon_center - lon_span/2
        lon_max = lon_center + lon_span/2
        subset = ds[var_name].sel(lat=slice(lat_max, lat_min), lon=slice(lon_min, lon_max))
    else:
        subset = ds[var_name]
    
    lats = subset["lat"].values
    lons = subset["lon"].values
    elev = subset.values
    
    # Flatten and create DataFrame
    lat_grid, lon_grid = np.meshgrid(lats, lons, indexing="ij")
    df = pd.DataFrame({
        "lat": lat_grid.ravel(),
        "lng": lon_grid.ravel(),
        "alt": elev.ravel()
    })
    df = df.dropna()
    df.to_csv(csv_file, index=False)
    print(f"Saved CSV with {len(df)} points to {csv_file}")


def read_csv_data(csv_file):
    """
    Read CSV data with columns 'lat', 'lng', 'alt'.
    
    Returns:
        lat, lng, alt (numpy arrays)
    """
    df = pd.read_csv(csv_file)
    return df['lat'].values, df['lng'].values, df['alt'].values


def create_meshgrid(lat, lng, alt, resolution=300, method="cubic"):
    """
    Convert flat lat/lng/alt arrays into meshgrid suitable for plotting.
    
    Parameters:
        resolution (int): Number of points along each axis for interpolation.
        method (str): Interpolation method: 'cubic', 'linear', 'nearest'.
    
    Returns:
        lat_grid, lon_grid, elev_grid (2D arrays)
    """
    # Original points
    points = np.column_stack((lat, lng))
    values = alt

    # Create fine grid
    lat_min, lat_max = lat.min(), lat.max()
    lon_min, lon_max = lng.min(), lng.max()
    lat_grid, lon_grid = np.meshgrid(
        np.linspace(lat_min, lat_max, resolution),
        np.linspace(lon_min, lon_max, resolution),
        indexing="ij"
    )

    # Interpolate elevation
    elev_grid = griddata(points, values, (lat_grid, lon_grid), method=method)
    return lat_grid, lon_grid, elev_grid


def plot_3d(lat_grid, lon_grid, elev_grid, save=False):
    fig = plt.figure(figsize=(10,8))
    ax = fig.add_subplot(111, projection="3d")
    ax.plot_surface(lon_grid, lat_grid, elev_grid, cmap="terrain", linewidth=0, antialiased=True)
    ax.set_xlabel("Longitude")
    ax.set_ylabel("Latitude")
    ax.set_zlabel("Elevation (m)")
    ax.set_title("3D Topography - Comodoro Rivadavia")
    ax.view_init(elev=30, azim=-60)
    if save:
        plt.savefig("topography_3d.png", dpi=300)
        print("Saved 3D plot as topography_3d.png")
    plt.show()


def plot_2d(lat_grid, lon_grid, elev_grid, positions=[], save=False):
    plt.figure(figsize=(8,6))
    plt.contourf(lon_grid, lat_grid, elev_grid, cmap="terrain")
    plt.colorbar(label="Elevation (m)")
    plt.xlabel("Longitude")
    plt.ylabel("Latitude")
    plt.title("2D Topography - Comodoro Rivadavia")
    if positions:
        place_lats = [p[0] for p in positions]
        place_lons = [p[1] for p in positions]
        place_names = [p[2] for p in positions]
        plt.scatter(place_lons, place_lats, color="red", marker="o")
        for (lat, lon, name) in positions:
            plt.text(lon, lat, name, color="black", fontsize=9, ha="right")
    if save:
        plt.savefig("topography_2d.png", dpi=300)
        print("Saved 2D plot as topography_2d.png")
    plt.show()