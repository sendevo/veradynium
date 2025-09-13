import xarray as xr
import numpy as np
import csv

def nc_to_csv(nc_file, csv_file, lat_center=None, lon_center=None, lat_span=0.2, lon_span=0.2):
    """
    Read a NetCDF DEM file, save cropped/full data to CSV, and return as list of dicts.
    """
    with xr.open_dataset(nc_file) as ds:
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

    # Flatten once
    lat_grid, lon_grid = np.meshgrid(lats, lons, indexing="ij")

    result = []
    with open(csv_file, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["lat", "lng", "alt"])  # header
        for lat, lon, alt in zip(lat_grid.ravel(), lon_grid.ravel(), elev.ravel()):
            if not np.isnan(alt):
                row = {"lat": float(lat), "lng": float(lon), "alt": float(alt)}
                result.append(row)
                writer.writerow([row["lat"], row["lng"], row["alt"]])

    print(f"Saved CSV to {csv_file}")
    return result
