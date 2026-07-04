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
       volume.SetMute(1, None)

def read_serial():
    port = find_esp_port()
    if(port == None):
        print("ESP32 not found/connected")
    else:
        ser = serial.Serial(port, 115200, timeout=1)
        volume = get_volume_interface()
        while True:
            line = ser.readline()
            handle_command(line.decode(), volume)

if __name__ == "__main__":
    read_serial()

























