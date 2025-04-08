import sys
import os
from util import load_json
from network_metrics import get_device_params
from device_charts import plot_device_params_time_series, plot_device_params_histograms


DATA_PATH = '../data/ttn_data.json'
CHARTS_DIR = 'charts'


if len(sys.argv) < 2:
    print("Please provide a device ID as a command line argument.")
    sys.exit(1)
else:
    data = load_json(DATA_PATH)
    device_id = sys.argv[1]
    device_params = get_device_params(data, device_id)
    #print(device_params)

    save_charts = sys.argv[2] if len(sys.argv) > 2 else None
    if save_charts:
        save_charts = save_charts.lower() == 'true'
    else:
        save_charts = False

    plot_device_params_time_series(device_params, device_id, CHARTS_DIR if save_charts else None)
    plot_device_params_histograms(device_params, device_id, CHARTS_DIR if save_charts else None)