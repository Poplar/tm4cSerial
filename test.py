import struct
import serial
import time

strPort = '/dev/icdi'
speedin = 20

ser = serial.Serial(port=strPort,baudrate=115200,timeout=.1)
ser.flush()
ser.write(struct.pack('>BBBffB',255,255,1,speedin,(2*speedin), 253))

while True:
    line = ser.read(size=12)
    print(line.encode('hex'))
    time.sleep(0.1)
