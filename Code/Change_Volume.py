import serial
import serial.tools.list_ports
from pycaw.pycaw import AudioUtilities

def find_esp_prot():
    key = "CP210x"
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if(key in port.description):
            return port.name

def get_volume_interface():
    device = AudioUtilities.GetSpeakers()
    return device.EndpointVolume
    
def read_serial():
    port = find_esp_prot()
    ser = serial.Serial(port, 115200, timeout=1)
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

























