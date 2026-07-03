import serial
import serial.tools.list_ports
from pycaw.pycaw import AudioUtilities
import requests

weather_API_file_path = r"C:\Users\ariel\OneDrive\שולחן העבודה - Copy\שולחן העבודה\Electronics Projects\Weather_API_Key.txt"

def get_API_Key(file_path):
    with open(file_path, 'r') as file:
        return file.read().strip()

def get_weather(api_key=get_API_Key(weather_API_file_path)):
    url = "https://api.openweathermap.org/data/2.5/weather"
    params = {
        "q": "Even Yehuda,IL",
        "appid": api_key,
        "units": "metric"
    }
    response = requests.get(url, params=params)
    data = response.json()
    return data

def find_esp_prot():
    key = "CP210x"
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if(key in port.description):
            return port.name

def get_volume_interface():
    device = AudioUtilities.GetSpeakers()
    return device.EndpointVolume
    
def handle_command(line, volume):
    line = line.strip()
    if (line == "VOL:+"):
       volume.VolumeStepUp(None)
    elif (line == "VOL:-"):
       volume.VolumeStepDown(None)
    elif (line == "VOL:MUTE"):
       volume.SetMute(1, None)

def read_serial():
    port = find_esp_prot()
    if(port == None):
        print("ESP32 not found/connected")
    else:
        ser = serial.Serial(port, 115200, timeout=1)
        volume = get_volume_interface()
        while True:
            line = ser.readline()
            handle_command(line.decode(), volume)

read_serial()

























