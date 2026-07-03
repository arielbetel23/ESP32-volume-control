import serial
import serial.tools.list_ports
from pycaw.pycaw import AudioUtilities


def get_volume_interface():
    devices = AudioUtilities.GetSpeakers()
    device = AudioUtilities.GetSpeakers()
    return device.EndpointVolume
    

volume = get_volume_interface()
print(volume.GetMasterVolumeLevelScalar())

def list_ports():
    ports = serial.tools.list_ports.comports()
    for port in ports:
        print(port.device)
        print(port.description)

def read_serial():
    ser = serial.Serial("COM3", 115200, timeout=1)
    while True:
        line = ser.readline()
        print(line.decode())





