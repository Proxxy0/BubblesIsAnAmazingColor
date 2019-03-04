import smbus
import time
import numpy as np
import os

clear = lambda: os.system('clear')

#current height--this is the processed value of the raw heights
cH = 0

#This is the width of our average
window = 10

#This is the index counter for the circular height array
counter = 0
print(counter)

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

    #time.sleep(.001)

    # MPL3115A2 address, 0x60(96)
        # Read data back from 0x00(00), 6 bytes
            # status, tHeight MSB1, tHeight MSB, tHeight LSB, temp MSB, temp LSB
    data = bus.read_i2c_block_data(0x60, 0x00, 6)

    # Convert the data to 20-bits
    tHeight = ((data[1] * 65536) + (data[2] * 256) + (data[3] & 0xF0)) / 16
    altitude = tHeight / 16.0

    # Output data to screen
    print("Altitude : %.2f m" %altitude)

#----------------------------------------------------------------------------

#appendtoCircle creates and fills a circular array of heights. This
#is the "window" that shifts across the data points. The values
#are overwritten as more samples are taken. This way, only 10 data
#points are stored.
rawHeightCircle = np.zeros(window)
def appendtoCircle(sample, counter):
    rawHeightCircle[counter] = sample
    counter = counter+1
    if(counter is window):
        counter = 0

def updateHeight():
    cH = np.average(rawHeightCircle)

def gatherHeight(counter):
    appendtoCircle(pingAlt(),counter)
    updateHeight()

#Initialize the first window of data points!
for index in rawHeightCircle:
    appendtoCircle(pingAlt(),counter)

#Get an initial average
iH = updateHeight()
print("Initial Height:", iH)

#Go into the main program loop
while(True):
    gatherHeight(counter)
    print("Current Height:", cH)
    clear()
