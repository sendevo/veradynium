from datetime import datetime, timedelta, timezone
import csv
import sys
from util import load_json, haversine


DATA_PATH = '../data/ttn_data_samples.json'
LOCATIONS_CSV_PATH = '../data/samples_locations.csv'
GATEWAY_LAT = -45.76979
GATEWAY_LNG = -67.48883



def load_location_samples(csv_path):
    """
    Load location samples from a CSV file into a list of tuples.
    
    Arguments:
        csv_path (str): Path to the CSV file.
    
    Returns:
        list of tuples: [(timestamp_str, lat, lng), ...]
    """
    location_samples = []
    with open(csv_path, newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            timestamp = row['time'].strip()
            lat = float(row['lat'].strip())
            lng = float(row['lng'].strip())
            location_samples.append((timestamp, lat, lng))
    return location_samples


def data_to_csv_with_location(data, device_id, location_samples):
    """
    Convert data to CSV format and append nearest location (within ±1 minute).
    
    Arguments:
        data (list): List of data entries (from JSON).
        device_id (str): Device ID to filter messages.
        location_samples (list of tuples): Each tuple contains (timestamp_str, lat, lng).
    
    Returns:
        csv_data (str): CSV formatted string with appended lat,lng.
    """
    # Convert location sample times to datetime for comparison
    parsed_locations = [
        (datetime.strptime(time_str.strip(), '%Y-%m-%d %H:%M').replace(tzinfo=timezone.utc), lat, lng)
        for time_str, lat, lng in location_samples
    ]

    csv_data = "timestamp,rssi,snr,sf,airtime,lat,lng,distance\n"

    for entry in data:
        if entry['result']['end_device_ids']['device_id'] == device_id:
            received_at = entry['result']['uplink_message']['received_at']
            dt = datetime.fromisoformat(received_at.replace('Z', '+00:00'))
            timestamp = dt.strftime('%Y-%m-%d %H:%M:%S')

            # Find closest location within ±1 minute
            closest_location = ("", "")
            min_diff = timedelta(minutes=1, seconds=0)  # 1-minute threshold

            for loc_time, lat, lng in parsed_locations:
                time_diff = abs(dt - loc_time)
                if time_diff <= min_diff:
                    min_diff = time_diff
                    closest_location = (lat, lng)

            if closest_location == ("", ""):
                continue  # skip message with no matching location

            metadata = entry['result']['uplink_message']['rx_metadata'][0]
            rssi = metadata['rssi']
            snr = metadata['snr']
            sf = entry['result']['uplink_message']['settings']['data_rate']['lora']['spreading_factor']
            airtime = entry['result']['uplink_message']['consumed_airtime']

            lat, lng = closest_location
            
            distance_to_gw = haversine(GATEWAY_LAT, GATEWAY_LNG, lat, lng)

            csv_data += f"{timestamp},{rssi},{snr},{sf},{airtime},{lat},{lng},{distance_to_gw}\n"

    return csv_data


def export_analysis_data(device_id):
    data = load_json(DATA_PATH)
    location_samples = load_location_samples(LOCATIONS_CSV_PATH)
    csv_data = data_to_csv_with_location(data, device_id, location_samples)
    return csv_data


if __name__ == "__main__":
    device_id = ''
    if len(sys.argv) < 2:
        print("Please provide a device ID as a command line argument.")
        sys.exit(1)
    else:
        device_id = sys.argv[1]
        csv_data = export_analysis_data(device_id)
        print(csv_data)