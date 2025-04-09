import requests
import json
import sys
import os
from dotenv import load_dotenv


if len(sys.argv) < 2:
    print("Please provide a file name as a command line argument.")
    sys.exit(1)

response_file_name = sys.argv[1]
print(f"Response will be saved in: {response_file_name}")

load_dotenv() # Define variables in .env file
#ACCESS_TOKEN = os.environ.get("ACCESS_TOKEN")
#API_ENDPOINT = os.environ.get("API_ENDPOINT")
ACCESS_TOKEN = os.environ.get("ACCESS_TOKEN_TEST")
API_ENDPOINT = os.environ.get("API_ENDPOINT_TEST")

headers = {
    'Authorization': f'Bearer {ACCESS_TOKEN}',
    'Content-Type': 'application/json'
}

response = requests.get(API_ENDPOINT, headers=headers)
print(response.text)
if response.status_code == 200:
    try:
        messages_data = []
        json_objects = response.text.splitlines()

        for json_str in json_objects:
            if json_str.strip():
                item = json.loads(json_str)
                messages_data.append(item)

        with open(response_file_name, 'w') as json_file:
            json.dump(messages_data, json_file, indent=4)

        print(f"Data saved successfully")

    except json.JSONDecodeError as e:
        print(f"Error while decoding JSON response: {e}")
        print("Content:")
        print(response.text)
else:
    print(f"Request error: {response.status_code}, {response.text}")