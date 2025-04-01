import sys
import os
from util import load_json
from device_metrics import get_device_params
from device_charts import plot_device_params_time_series, plot_device_params_histograms


DATA_PATH = '../data/ttn_data.json'
CHARTS_DIR = 'charts'
SAVE_CHARTS = True


if len(sys.argv) < 2:
    print("Please provide a device ID as a command line argument.")
    sys.exit(1)
else:
    data = load_json(DATA_PATH)
    device_id = sys.argv[1]
    device_params = get_device_params(data, device_id)
    #print(device_params)

    if SAVE_CHARTS:
        if not os.path.exists(CHARTS_DIR):
            os.makedirs(CHARTS_DIR)
        
        chart_path = ''
        time_series_path = os.path.join(CHARTS_DIR, f'{device_id}_time_series.png')
        histograms_path = os.path.join(CHARTS_DIR, f'{device_id}_histograms.png')

        plot_device_params_time_series(device_params, time_series_path)
        plot_device_params_histograms(device_params, histograms_path)
    else:
        plot_device_params_time_series(device_params)
        plot_device_params_histograms(device_params)
    