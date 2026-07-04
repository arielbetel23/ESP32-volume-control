import serial
import serial.tools.list_ports
from pycaw.pycaw import AudioUtilities

def find_esp_port():
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
        volume.SetMute(0 if volume.GetMute() else 1, None)



























