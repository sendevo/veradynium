import xarray as xr
import numpy as np
import csv

def regrid_to_size(da, target_lat, target_lon):
    new_lats = np.linspace(float(da.lat.min()), float(da.lat.max()), target_lat)
    new_lons = np.linspace(float(da.lon.min()), float(da.lon.max()), target_lon)
    return da.interp(lat=new_lats, lon=new_lons)

def nc_to_csv(
    nc_file,
    csv_file,
    lat_center=None,
    lon_center=None,
    lat_span=0.2,
    lon_span=0.2,
    grid_size=None  # subsample factor (keep every Nth point)
):
    with xr.open_dataset(nc_file, engine="netcdf4") as ds:
        var_name = list(ds.data_vars.keys())[0]
        da = ds[var_name]

        if lat_center is not None and lon_center is not None:
            lat_min = lat_center - lat_span/2
            lat_max = lat_center + lat_span/2
            lon_min = lon_center - lon_span/2
            lon_max = lon_center + lon_span/2
            da = da.sel(lat=slice(lat_max, lat_min), lon=slice(lon_min, lon_max))

        if grid_size is not None:
            target_lat, target_lon = grid_size
            nlat = da.sizes["lat"]
            nlon = da.sizes["lon"]
            fac_lat = max(1, nlat // target_lat)
            fac_lon = max(1, nlon // target_lon)
            da = da.coarsen(lat=fac_lat, lon=fac_lon, boundary="trim").mean()

        lats = da["lat"].values
        lons = da["lon"].values
        elev = da.values

    result = []

    with open(csv_file, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["lat", "lng", "alt"])

        for i, lat in enumerate(lats):
            for j, lon in enumerate(lons):
                alt = elev[i, j]
                if not np.isnan(alt):
                    row = {"lat": float(lat), "lng": float(lon), "alt": float(alt)}
                    result.append(row)
                    writer.writerow([row["lat"], row["lng"], row["alt"]])

    return result
