from util import load_json
import sys


DATA_PATH = '../data/ttn_data.json'
"""
Data format:
[
    {
        "result": {
            "end_device_ids": {
                "device_id": "",
                "application_ids": {
                    "application_id": ""
                },
                "dev_eui": "",
                "dev_addr": ""
            },
            "received_at": "",
            "uplink_message": {
                "rx_metadata": [
                    {
                        "gateway_ids": {
                            "gateway_id": "",
                            "eui": ""
                        },
                        "time": "",
                        "timestamp": 0,
                        "rssi": 0,
                        "channel_rssi": 0,
                        "snr": 0,
                        "frequency_offset": "",
                        "channel_index": 0,
                        "gps_time": "",
                        "received_at": ""
                    }
                ],
                "settings": {
                    "data_rate": {
                        "lora": {
                            "bandwidth": 0,
                            "spreading_factor": 0,
                            "coding_rate": ""
                        }
                    },
                    "frequency": "",
                    "timestamp": 0,
                    "time": ""
                },
                "received_at": "",
                "confirmed": true,
                "consumed_airtime": "000s",
                "network_ids": {
                    "net_id": "",
                    "ns_id": "",
                    "tenant_id": "",
                    "cluster_id": "",
                    "cluster_address": "",
                    "tenant_address": ""
                }
            }
        }
    },
    {...}
]
"""

def get_device_params(data, device_id):
    """
    Builds a dict with parameters of interest from the dataset. 
    Arguments:
        data (list): List of data entries.
        device_id (str): Device ID.
    Returns:
        device_params (dict): Dictionary containing device parameters.
            {
                'rssi': [rssi_values],
                'snr': [snr_values],
                'sf': [sf_values],
                'airtime': [airtime_values]
            }
    """

    device_params = {} # Format {rssi: [rssi_values], snr: [snr_values], sf: [sf_values], airtime: [airtime_values]}
    for entry in data:
        if entry['result']['end_device_ids']['device_id'] == device_id:

            metadata = entry['result']['uplink_message']['rx_metadata'][0]

            if 'snr' in metadata:
                snr = metadata['snr']            
                if 'snr' not in device_params:
                    device_params['snr'] = []
                device_params['snr'].append(snr)

            if 'rssi' in metadata:
                rssi = metadata['rssi']
                if 'rssi' not in device_params:
                    device_params['rssi'] = []
                device_params['rssi'].append(rssi)

            if 'settings' in entry['result']['uplink_message']:
                settings = entry['result']['uplink_message']['settings']
                if 'data_rate' in settings:
                    data_rate = settings['data_rate']
                    if 'lora' in data_rate:
                        lora = data_rate['lora']
                        if 'spreading_factor' in lora:
                            sf = lora['spreading_factor']
                            if 'sf' not in device_params:
                                device_params['sf'] = []
                            device_params['sf'].append(sf)

            if 'consumed_airtime' in entry['result']['uplink_message']:
                airtime = entry['result']['uplink_message']['consumed_airtime']
                if 'airtime' not in device_params:
                    device_params['airtime'] = []
                device_params['airtime'].append(airtime)

    return device_params




if __name__ == "__main__":
    data = load_json(DATA_PATH)

    device_id = ''
    if len(sys.argv) < 2:
        print("Not device ID provided, printing data stats.")
        #messages_stats = get_data_stats(data)
        #print(messages_stats)
        sys.exit(1)
    else:
        device_id = sys.argv[1]
        device_params = get_device_params(data, device_id)
        print(device_params)    

