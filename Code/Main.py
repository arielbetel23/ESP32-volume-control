import time
import threading
import serial
from Change_Volume import find_esp_port, get_volume_interface, handle_command
from Send_Data import send_time_and_date, send_weather