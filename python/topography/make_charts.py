from utils import nc_to_csv, srtm_to_csv, read_csv_data, create_meshgrid_interpolation, create_meshgrid_reshape, plot_3d, plot_2d

lat_center=-45.86 
lon_center=-67.475
lat_span=0.21
lon_span=0.25

#nc_to_csv("data/S46W068.SRTMGL1_NC.nc", "topography_nasa.csv", lat_center=-45.86, lon_center=-67.475, lat_span=0.21, lon_span=0.25)

#srtm_to_csv("topography_srtm.csv", lat_center, lon_center, lat_span, lon_span, step=0.001)

# Combine CSVs
#df1 = pd.read_csv("topography_nasa.csv")
#df2 = pd.read_csv("topography_srtm.csv")
#df_all = pd.concat([df1, df2]).drop_duplicates(subset=["lat", "lng"]).reset_index(drop=True)
#df_all.to_csv("data/topography_all.csv", index=False)
#print("Combined CSV saved as data/topography_all.csv")

# Read CSV
lat, lng, alt = read_csv_data("data/topography_nasa.csv")

# Convert to meshgrid for plotting

# Plot 3D surface
#plot_3d(lat_grid, lon_grid, elev_grid)

# Add locations
locations = [
    (-45.785444, -67.468065, "Aeropuerto"),
    (-45.769812, -67.488767, "Km 17"),
    (-45.824869, -67.463616, "UNPSJB"),
    (-45.927944, -67.560873, "Rada Tilly"),
    (-45.869743, -67.546032, "Moure"),
    (-45.856229, -67.479608, "Chenque"),
    (-45.789584, -67.431268, "Km 8"),
    (-45.778171, -67.364135, "Farallón"),
    (-45.959023, -67.535539, "Punta Marqués"),
    (-45.828837, -67.542404, "Laprida") 
]

#lat_grid, lon_grid, elev_grid = create_meshgrid_interpolation(lat, lng, alt)
#print(f"lat_grid shape: {lat_grid.shape}")
#print(f"lon_grid shape: {lon_grid.shape}")
#print(f"elev_grid shape: {elev_grid.shape}")
#plot_2d(lat_grid, lon_grid, elev_grid, locations, file_path="topography_2d_interpolation.png")

lat_grid, lon_grid, elev_grid = create_meshgrid_reshape(lat, lng, alt)
print(f"lat_grid shape: {lat_grid.shape}")
print(f"lon_grid shape: {lon_grid.shape}")
print(f"elev_grid shape: {elev_grid.shape}")
plot_2d(lat_grid, lon_grid, elev_grid, locations, file_path="topography_2d_reshape.png")
