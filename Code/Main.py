import time
import threading
import serial
from Change_Volume import find_esp_port, get_volume_interface, handle_command
from Send_Data import send_time_and_date, send_weather


def reader(ser, volume):
    while True:
        line = ser.readline().decode(errors="ignore")
        if line:
            handle_command(line, volume)

def writer(ser):
    last_weather = 0
    while True:
        send_time_and_date(ser)
        if time.time() - last_weather >= 60:
            send_weather(ser)
            last_weather = time.time()
        time.sleep(1)

def main():
    port = find_esp_port()
    if port is None:
        print("ESP32 not found/connected")
        return
    ser = serial.Serial(port, 115200, timeout=1)
    volume = get_volume_interface()
    threading.Thread(target=reader, args=(ser, volume), daemon=True).start()
    writer(ser)
























