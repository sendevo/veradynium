# Veradynium


### Data analysis workflow
1.- Create a .env file in the python folder and define the following variables depending on the application (from TTN) to use:

```bash
ACCESS_TOKEN
API_ENDPOINT
```

2.- Install the required libraries:

```bash
cd python
source venv/bin/activate
pip3 install -r requirements.txt
```

3.- Download data from TTN with ```api_call.py```, for example to ```ttn_data.json``` file:

``` bash
python3 api_call.py ttn_data.json
```

4.- Overview the content of the downloaded data. In ```network_metrics.py``` define the ```DATA_PATH``` variable with the path to the downloaded data file.  

``` bash
python3 network_metrics.py
``` 

5.- Overview specific information about and end-device. For example for ```ed_1234```:

```bash
python3 network_metrics.py ed_1234
```

6.- Plot parameters from specific end-device. Define the file path and a directory to save charts in the same script. Use ```--save``` to save data to a file. For example for ```ed_1234```:

```bash
python3 device_charts.py ed_1234 --save
```

7.- For the coverage analysis, moving an end-device over different locations, and when the end-device reports data, write the timestamp, lat and lng in a CSV file, for example:

```bash
time,lat,lng
2023-10-01 12:00:00,40.7128,-74.0060
2023-10-01 12:05:00,40.7138,-74.0070
2023-10-01 12:10:00,40.7148,-74.0080
```

Then define the json file with the data reported by the end-device, the csv file with locations and define the gateway location in the ```location_analysis.py``` file. It is assumed that all data was reported to the same gateway. If not, add the corresponding filters. Run the script:

```bash
python3 location_analysis.py ed_1234
```

This will print csv formatted data with the following columns:
timestamp, rssi, snr, sf, airtime, lat, lng, distance

### Solver
Work in progress...


### Dashboard
Work in progress...