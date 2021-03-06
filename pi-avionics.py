#!/usr/bin/python

import smbus
import time
import numpy as np
import os
import RPi.GPIO as GPIO
from datetime import datetime

clear = lambda: os.system('clear')

#INITIALIZE VARIABLES AND PIN STATES
GPIO.setmode(GPIO.BCM)
GPIO.setup(5, GPIO.OUT)
GPIO.setup(6, GPIO.OUT)
GPIO.setup(18, GPIO.OUT)

outputfile = open("/home/pi/Desktop/BubblesIsAnAmazingColor/outputData-{}.txt".format(datetime.now()),"a")

#starttime
starttime = time.time()

#current height--this is the processed value of the raw heights
cH = 0
maxH = 0
cT = time.time()
prevH = 0
prevT = 0
cV = 0
prevV = 0
cA = 0
maxHT = 0
mainFlag = 0
drogueFlag = 0

#This is the width of our average
window = 10

#This is the index counter for the circular height array
counter = 0


#Run Startup Tones-----------------------------------------------------------
startfreq = 400
pwm = GPIO.PWM(18, startfreq)
pwm.start(50)
time.sleep(0.5)
for frequency in np.arange(100):
    pwm.start(50)
    time.sleep(0.005)
    pwm.ChangeFrequency(startfreq+frequency)
    endfreq = startfreq+frequency
#for frequency in np.arange(800):
#    pwm.ChangeFrequency(endfreq-frequency)
#    time.sleep(0.005)
pwm.stop()
#----------------------------------------------------------------------------


#----------------------------------------------------------------------------
#pingAlt is code supplied from ControlEverything's altimeter source
#Final State: set the altitude variable.
def pingAlt():
    # Get I2C bus
    bus = smbus.SMBus(1)

    # MPL3115A2 address, 0x60(96)
    # Select control register, 0x26(38)
    #               0xB9(185)       Active mode, OSR = 128, Altimeter mode
    bus.write_byte_data(0x60, 0x26, 0xB9)
    # MPL3115A2 address, 0x60(96)
    # Select data configuration register, 0x13(19)
    #               0x07(07)        Data ready event enabled for altitude, pressure, temperature
    bus.write_byte_data(0x60, 0x13, 0x07)
    # MPL3115A2 address, 0x60(96)
    # Select control register, 0x26(38)
    #               0xB9(185)       Active mode, OSR = 128, Altimeter mode
    bus.write_byte_data(0x60, 0x26, 0xB9)

   #time.sleep(.5)

    # MPL3115A2 address, 0x60(96)
        # Read data back from 0x00(00), 6 bytes
            # status, tHeight MSB1, tHeight MSB, tHeight LSB, temp MSB, temp LSB
    data = bus.read_i2c_block_data(0x60, 0x00, 6)

    # Convert the data to 20-bits
    tHeight = ((data[1] * 65536) + (data[2] * 256) + (data[3] & 0xF0)) / 16
    altitude = tHeight / 16.0

    # Output data to screen
    #print("Altitude : %.2f m" %altitude)
    return(altitude)
#----------------------------------------------------------------------------

#UPDATE FUNCTIONS
#--------------------------------------------------------------------
#appendtoCircle creates and fills a circular array of heights. This
#is the "window" that shifts across the data points. The values
#are overwritten as more samples are taken. This way, only 10 data
#points are stored.
RHC = np.zeros(window) #Raw Height Circle
def appendtoCircle(sample,counter,RHC):
    RHC[counter] = sample

def getH(RHC,cH):
    cH = np.average(RHC)
    return(cH)

def getT(cT):
    cT = time.time()
    return(cT)

def getV(prevH,prevT,cH,cT):
    cV = (cH-prevH)/(cT-prevT)
    return cV

def getA(prevV,prevT,cV,cT):
    cA = (cV-prevV)/(cT-prevT)
    return cA

def inc(counter):
    counter = counter+1
    if(counter is window):
        counter = 0
    return(counter)
#--------------------------------------------------------------------

def endFlight(outputfile):
    outputfile.close()

def Drogue():
    GPIO.output(5, HIGH)
def Main():
    GPIO.output(6, HIGH)

#BEGIN PROGRAM
#-------------------------------------------------------------------
#INITIALIZATION STEP--------------------------
#Initialize the first window of data points!
for index in RHC:
    appendtoCircle(pingAlt(),counter,RHC)
    counter = inc(counter)

#Get an initial average
iH = getH(RHC,cH)
print("Initial Height:", iH)
#---------------------------------------------

pwm.ChangeFrequency(1500)
#Go into the main program loop
beepcount = 0
finalcount = 0
while(finalcount <50):
    
    appendtoCircle(pingAlt(),counter,RHC)
    counter = inc(counter)
    prevH = cH
    cH = getH(RHC,cH)
    prevT = cT
    cT = getT(cT)
    prevV = cV
    cV = getV(prevH,prevT,cH,cT)
    cA = getA(prevV,prevT,cV,cT)

    if(cH > prevH):
	maxH = cH
	maxHT = cT

    print(RHC)
    print("Current Height:", cH, " Count: ", counter)
    outputstring = ""+str(cH)+"\t"+str(time.time()-starttime)+"\n"
    outputfile.write(outputstring)

    if beepcount is 200:
        pwm.start(50)
    if beepcount is 225:
        pwm.stop()
        beepcount = 0
    beepcount = beepcount+1
    clear()
    finalcount = finalcount+1
    
    if(cH < maxH and (cT+5) > maxHT and (cH - iH) > 150):
	Drogue()
	drogueFlag = 1
	outputfile.write("\n \n Drogue \n \n"+str(cT))
    if(cH < 150 and drogueFlag is 1):
	Main()
	mainFlag = 1
	outputfile.write("\n \n Main \n \n"+str(cT))

    if(mainFlag is 1 and 2 > cV > -2):
	endflight(outputfile)

endflight(outputfile)
