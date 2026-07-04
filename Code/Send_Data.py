import time
import serial
from Change_Volume import find_esp_port
from Get_Weather import extract_weather
from Get_Datetime import get_date_and_time

def send_time(ser):
    date, time_str = get_date_and_time()
    ser.write(f"DATE:{date}\n".encode())
    ser.write(f"TIME:{time_str}\n".encode())










