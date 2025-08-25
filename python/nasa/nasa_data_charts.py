from utils import nc_to_csv, read_csv_data, create_meshgrid, plot_3d, plot_2d


# 1. Convert NetCDF to CSV
#nc_to_csv("S46W068.SRTMGL1_NC.nc", "comodoro.csv", lat_center=-45.86, lon_center=-67.475, lat_span=0.21, lon_span=0.25)

# 2. Read CSV
lat, lng, alt = read_csv_data("comodoro.csv")

# 3. Convert to meshgrid for plotting
lat_grid, lon_grid, elev_grid = create_meshgrid(lat, lng, alt, resolution=300)

# 4. Plot 3D surface
#plot_3d(lat_grid, lon_grid, elev_grid)

# 5. Plot 2D contour
positions = [
    (-45.785444, -67.468065, "Aeropuerto"),
    (-45.769812, -67.488767, "Km 17"),
    (-45.824869, -67.463616, "UNPSJB"),
    (-45.927944, -67.560873, "Rada Tilly"),
    (-45.856229, -67.479608, "Chenque"),
    (-45.789584, -67.431268, "Km 8"),
    (-45.778171, -67.364135, "Farallón"),
    (-45.959023, -67.535539, "Punta Marqués"),
    (-45.828837, -67.542404, "Laprida") 
]

plot_2d(lat_grid, lon_grid, elev_grid, positions, True)