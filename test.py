import struct
import serial
import time

CMD_STOP = 1
CMD_CLOSEDLOOP = 2
CMD_OPENLOOP = 3

def printLine(line):
    print(line.encode('hex')),
    cmd = line[2]
    vel = struct.unpack('<f', line[3:-5]) #/3:-1 for last change
    duty = struct.unpack('<f', line[7:-1])
    print "CMD=" + cmd.encode('hex') + " VEL=" + str(vel) + " DUTY=" + str(duty)
    

def writeLine(cmd, arg):
    ser.write(struct.pack('<BBBfB',255,255,cmd,arg,253))


def runTest(cmd, arg, iter):
    ser.reset_input_buffer()
    time.sleep(0.1)
    writeLine(cmd, arg)
    for i in range(iter):
        line = ser.read(size=12)
        printLine(line)
    writeLine(CMD_STOP, 0.0)
    print ""
    time.sleep(5)

    
strPort = '/dev/icdi'
speedin = 20.0

ser = serial.Serial(port=strPort,baudrate=115200)

runTest(CMD_CLOSEDLOOP, 10.0, 5)
runTest(CMD_CLOSEDLOOP, 20.0, 5)
