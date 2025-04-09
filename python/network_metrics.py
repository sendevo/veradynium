from util import load_json
import sys
from datetime import datetime


#DATA_PATH = '../data/ttn_data.json'
DATA_PATH = '../data/ttn_data_samples.json'
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

def get_data_stats(data, print_stats=False):
    """
    Get statistics from the dataset.
    Arguments:
        data (list): List of data entries.
        print_stats (bool): Whether to print the statistics or not.
    Returns:
        stats (dict): Dictionary containing statistics.
            {
                'time_window': {

                    'start_time': start_time,
                    'end_time': end_time,
                    'time_window_days': time_window_days
                },
                'num_devices': int,
                'num_messages': int,
                'num_gateways': int,
                'messages_per_device': {device_id: num_messages},
                'messages_per_gateway': {gateway_id: num_messages}
            }
    """
    
    # Get time window (YYYY-MM-DD format)
    # Get time window with full datetime (YYYY-MM-DD HH:MM:SS)
    time_window = []
    for entry in data:
        timestamp_str = entry['result']['uplink_message']['received_at']
        timestamp = datetime.fromisoformat(timestamp_str.replace('Z', '+00:00'))  # Handle 'Z' timezone
        time_window.append(timestamp)

    time_window.sort()
    start_time = time_window[0]
    end_time = time_window[-1]
    time_window_days = (end_time - start_time).days

    if print_stats:
        print(f"Time window: {start_time.strftime('%Y-%m-%d %H:%M')} to {end_time.strftime('%Y-%m-%d %H:%M')} ({time_window_days} days)")

    # Get number of messages
    num_messages = len(data)
    if print_stats:
        print(f"Number of messages: {num_messages}")

    # Get number of end devices
    devices = set()
    for entry in data:
        devices.add(entry['result']['end_device_ids']['device_id'])
    num_devices = len(devices)

    if print_stats:
        print(f"Number of devices: {num_devices}")

    # Get number of gateways
    gateways = set()
    for entry in data:
        metadata = entry['result']['uplink_message']['rx_metadata']
        for gateway in metadata:
            gateways.add(gateway['gateway_ids']['gateway_id'])
    num_gateways = len(gateways)
    if print_stats:
        print(f"Number of gateways: {num_gateways}")

    # Get number of messages per device
    messages_per_device = {}
    for entry in data:
        device_id = entry['result']['end_device_ids']['device_id']
        if device_id not in messages_per_device:
            messages_per_device[device_id] = 0
        messages_per_device[device_id] += 1
    if print_stats:
        print("\nNumber of messages per device:")
        for device_id, num_messages in messages_per_device.items():
            print(f"Device ID: {device_id}, Number of messages: {num_messages}")

    # Get number of messages per gateway
    messages_per_gateway = {}
    for entry in data:
        metadata = entry['result']['uplink_message']['rx_metadata']
        for gateway in metadata:
            gateway_id = gateway['gateway_ids']['gateway_id']
            if gateway_id not in messages_per_gateway:
                messages_per_gateway[gateway_id] = 0
            messages_per_gateway[gateway_id] += 1
    if print_stats:
        print("\nNumber of messages per gateway:")
        for gateway_id, num_messages in messages_per_gateway.items():
            print(f"Gateway ID: {gateway_id}, Number of messages: {num_messages}")
    
    return {
        'num_devices': num_devices,
        'num_messages': num_messages,
        'num_gateways': num_gateways,
        'messages_per_device': messages_per_device,
        'messages_per_gateway': messages_per_gateway,
        'time_window': {
            'start_time': start_time,
            'end_time': end_time,
            'time_window_days': time_window_days
        }
    }


def data_to_csv(data, device_id):
    """
    Convert data to CSV format.
    Arguments:
        data (list): List of data entries.
        device_id (str): Device ID.
    Returns:
        csv_data (str): CSV formatted string.
    """
    csv_data = "timestamp,rssi,snr,sf,airtime\n"
    for entry in data:
        if entry['result']['end_device_ids']['device_id'] == device_id:
            received_at = entry['result']['uplink_message']['received_at']
            formatted_timestamp = datetime.fromisoformat(received_at.replace('Z', '+00:00'))
            timestamp = formatted_timestamp.strftime('%Y-%m-%d %H:%M:%S')
            metadata = entry['result']['uplink_message']['rx_metadata'][0]
            rssi = metadata['rssi']
            snr = metadata['snr']
            sf = entry['result']['uplink_message']['settings']['data_rate']['lora']['spreading_factor']
            airtime = entry['result']['uplink_message']['consumed_airtime']
            csv_data += f"{timestamp},{rssi},{snr},{sf},{airtime}\n"
    return csv_data


if __name__ == "__main__":
    data = load_json(DATA_PATH)
    device_id = ''
    if len(sys.argv) < 2:
        print("Not device ID provided, printing data stats...", end='\n\n')
        get_data_stats(data, True)
        sys.exit(1)
    else:
        device_id = sys.argv[1]
        device_params = get_device_params(data, device_id)
        print(device_params)   

        csv_data = data_to_csv(data, device_id)
        print(f"\nMessages for device {device_id} in CSV format:\n")
        print(csv_data) 
