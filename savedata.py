import sys, serial, argparse, threading, struct 
#import numpy as np
from time import sleep
from collections import deque
startflag = False
stopflag = False
mode = 0
openspd = 0
closed = 0


def kbdinput():
    global stopflag
    global startflag
    global mode
    global closed
    global openspd

    while(stopflag == False):
        #flaginput = raw_input("Press enter to stop")
        flaginput = raw_input("Press O for open, C for closed, or S for stop: ")
        print(flaginput)
        if(flaginput == 'S'):
            print("Stopping motor controller")
            startflag = True
            stopflag = True
        if(flaginput == 'C'):
            #try:
            closedspeed = float(raw_input("Please enter a value between -2000 and 2000: "))
            #except:
            #    print("Invalid value, please try again")
            if(closedspeed < 2000 and closedspeed > -2000):
                print(closedspeed)
                startflag = True
                mode = 2
                closed = closedspeed #no race conditions
        if(flaginput == 'O'):
            #try:
            openspeed = float(raw_input("Please enter a value between -99 and 99: "))
            #except ValueError:
                #print("Invalid value, please try again")
            if(openspeed < 99 and openspeed > -99):
                startflag = True
                mode = 1
                openspd = openspeed #no race conditions


def main():
    global stopflag
    global startflag
    global mode 
    global openspd
    global closed
    strPort = '/dev/icdi'
    #try: 
    ser = serial.Serial(port=strPort,baudrate=115200,timeout=.1)
    #except:
        #print("Serial port did not open\n")
        #exit()
    #else:
        #print("Serial port opened\n")
    str1 = ''
    duty = 0
    #What is my input?
    #non-interactive for now
    #try:
        #speedin = float(input("Enter a floating point speed in m/s:"))
    #except ValueError:
        #print("Invalid number, quitting")
        #exit() 
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

    # UnboundLocalError is occuring on vel
    vel = 0
    
    while(startflag == False): #waits for input from kbd thread
       pass 
    while(stopflag == False): 
            if(startflag == True):
                if(mode == 1):
                    speedin = openspd
                    startflag = False
                if(mode == 2):
                    speedin = closed
                    startflag = False
                    
            #####UNCOMMENT THIS LINE, COMMENT NEXT######
            #ser.write(struct.pack('>BBBfB', 255,255,1, speedin, 253))
            ser.write(struct.pack('>BBBffB',255,255,1,speedin,(2*speedin), 253))
            line = ser.read(size=12)
            #line = ser.read(size=12) #for 2 floats
            print(line.encode('hex'))
            sleep(.1)
            
            while(len(line) < 12):
                if(stopflag == True):
                    print("Interrupted by keyboard input")
                    break
                ser.write(struct.pack('>BBBffB',255,255,1,speedin,(2*speedin), 253))
                print(line)
   #             print(send_data)
                print("Error: not enough bytes sent in 50 ms")
                line = ser.read(size=12)

            if(len(line) > 2 and ord(line[0]) == 255 and ord(line[1]) == 255):
                cmd = line[2]
                #vel = struct.unpack('>f', line[3:-1])
                vel = struct.unpack('>f', line[3:-5]) #/3:-1 for last change
                duty = struct.unpack('>f', line[7:-1])

                print(str(vel)[1:-2])
                print(str(duty)[1:-2])

            else:
                print("Error, wrong start sequence, or I suck at math.\n") 
            
            #speedin = plant_simulate(pwmin)
            str1 = str1 + str(vel)[1:-2] +' ' +str(duty)[1:-2] +  '\n'

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
