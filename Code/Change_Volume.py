import serial
import serial.tools.list_ports
from pycaw.pycaw import AudioUtilities

def list_ports():
    ports = serial.tools.list_ports.comports()
    for port in ports:
        print(port.device)
        print(port.description)

def get_volume_interface():
    device = AudioUtilities.GetSpeakers()
    return device.EndpointVolume
    
def read_serial():
    ser = serial.Serial("COM3", 115200, timeout=1)
    volume = get_volume_interface()
    while True:
        line = ser.readline()
        handle_command(line.decode(), volume)

def handle_command(line, volume):
    line = line.strip()
    if (line == "VOL:+"):
       volume.VolumeStepUp(None)
    elif (line == "VOL:-"):
       volume.VolumeStepDown(None)
    elif (line == "VOL:MUTE"):
       volume.SetMute(1, None)


read_serial()


























