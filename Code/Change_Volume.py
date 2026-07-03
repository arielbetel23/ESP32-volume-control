import serial.tools.list_ports

objects = serial.tools.list_ports.comports()

for i in objects:
    print(i.device)
    print(i.description)

serial.Serial("COM3", 115200, timeout=1)
while(True):
    print(serial.Serial("COM3", 115200, timeout=1).readline())
