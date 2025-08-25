import json
import csv
import io
from math import radians, sin, cos, sqrt, atan2

def load_json(file_path):  
    with open(file_path, 'r') as file:
        data = json.load(file)
    return data

def csv_string_to_columns(csv_string, col1_idx=0, col2_idx=1):
    f = io.StringIO(csv_string)
    reader = csv.reader(f)
    header = next(reader)  # skip header
    x = []
    y = []
    for row in reader:
        if len(row) > max(col1_idx, col2_idx):  # check row length
            x.append(float(row[col1_idx]))
            y.append(float(row[col2_idx]))
    return x, y


def haversine(lat1, lon1, lat2, lon2):
    R = 6371
    lat1, lon1, lat2, lon2 = map(radians, [lat1, lon1, lat2, lon2])
    dlat = lat2 - lat1
    dlon = lon2 - lon1

    a = sin(dlat/2)**2 + cos(lat1) * cos(lat2) * sin(dlon/2)**2
    c = 2 * atan2(sqrt(a), sqrt(1 - a))
    return R * c