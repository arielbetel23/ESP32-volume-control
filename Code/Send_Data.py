import time
import serial
from Change_Volume import find_esp_port
from Get_Weather import extract_weather
from Get_Datetime import get_date_and_time


def send_time_and_date(ser):
    date, time_str = get_date_and_time()
    ser.write(f"DATE:{date}\n".encode())
    ser.write(f"TIME:{time_str}\n".encode())

def send_weather(ser):
    temp, desc = extract_weather()
    ser.write(f"TEMP:{temp}\n".encode())
    ser.write(f"DESC:{desc}\n".encode())


port = find_esp_port()
if(port == None):
    print("ESP32 not found/connected")
else:
    ser = serial.Serial(port,115200, timeout=1)
    last_weather = time.time()
    send_weather(ser)
    while True:
        send_time_and_date(ser)
        if(time.time() - last_weather >= 60):
            send_weather(ser)
            last_weather = time.time()
        time.sleep(1)




