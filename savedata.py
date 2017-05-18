import sys, serial, argparse, threading, struct 
#import numpy as np
from time import sleep
from collections import deque
stopflag = False
ser = 0
def kbdinput():
    global stopflag
    flaginput = input("Press enter to stop")
    print("I'll stop now\n")
    stopflag = True

def main():
    global stopflag
    strPort = '/dev/ttyACM0'
    #try: 
    ser = serial.Serial(port=strPort,baudrate=115200,timeout=.1)
    #except:
        #print("Serial port did not open\n")
        #exit()
    #else:
        #print("Serial port opened\n")
    str1 = ''
    #What is my input?
    #non-interactive for now
    try:
        speedin = float(input("Enter a floating point speed in m/s:"))
    except ValueError:
        print("Invalid number, quitting")
        exit() 
    PWM = open('PWM','r+')
    print("\n")
    kbdinput_thread = threading.Thread(target=kbdinput)
    kbdinput_thread.start()
    #send packet - 2 packing bytes, cmd = 1, speedin, chksum
    #send_data = struct.pack('>BBBfB',255,255,1,speedin, 253)
    #send_data = struct.pack('>BBBBBB', 255,255,72,69,76,76)
    #first loop
    #for x in range(1, 30000): #reads in 100 data points.
    #ser.write(send_data)
    while(stopflag == False): 
            ser.write(struct.pack('>BBBfB',255,255,1,speedin, 253))
            line = ser.read(size=8)
            print(line)
            sleep(.1)
            while(len(line) < 8):
                if(stopflag == True):
                    print("Interrupted by keyboard input\n")
                    break
                ser.write(send_data)
                print(line)
                print(send_data)
                print('\n')
                print("Error: not enough bytes sent in 50 ms\n")
                line = ser.read(size=8)

            if(ord(line[0]) == 255 and ord(line[1]) == 255):
                cmd = line[2]
                #pwmin = (line[3] << 4) + line[4]
                pwmin = struct.unpack('>f', line[3:-1])
                print(pwmin)

            else:
                print("Error, wrong start sequence, or I suck at math.\n") 
            
            #speedin = plant_simulate(pwmin)
            str1 = str1 + str(pwmin) + '\n'

    #clean()
    # close serial
    ser.flush()
    ser.close()
    #Write data
    PWM.seek(0)
    PWM.write(str1)
    PWM.truncate()
    PWM.close()
    exit()
#Entry point basically?
if __name__ == '__main__':
    main()
