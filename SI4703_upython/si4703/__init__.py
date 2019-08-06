from .constants import *
from machine import Pin, I2C
import machine
from i2c import I2CAdapter
import time


class SI4703():
    def __init__(self, i2c_addr, i2c_device=None):
        print("__init__")

        self.i2c_addr = i2c_addr
        self._i2c = i2c_device
        self.si4703_registers = [0] * 16

        self.activate_two_wire()
        self.init_chip()
        self.goToChannel()
        self.enable_RDS()
        self.setVolume()

    def enable_RDS(self):
        print("enable rds")
        self.readRegisters()
        self.si4703_registers[SYSCONFIG1] |= (1<<12)
        self.updateRegisters()


    def activate_two_wire(self):
        # init code needs to activate 2-wire (i2c) mode
        # the si4703 will not show up in i2cdetect until
        # you do these steps to put it into 2-wire (i2c) mode
        print("activate_two_wire")
        nRST = machine.Pin(0, machine.Pin.OUT) # D3 / GPIO0
        sdiopin = machine.Pin(4, machine.Pin.OUT) # SDA

        sdiopin.off()
        time.sleep(0.1)
        nRST.off()
        time.sleep(0.1)
        nRST.on()
        time.sleep(0.1)

        self._i2c = I2CAdapter(scl=Pin(5), sda=Pin(4), freq=100000)
        print("two wire activated, address: ", self._i2c.scan())


    def init_chip(self):
        print("init_chip")

        self.readRegisters() # Read the current register set
        self.si4703_registers[0x07] = 0x8100 # Enable the oscillator
        self.updateRegisters() # Update

        time.sleep(0.5) # Wait for clock to settle - from AN230 page 9

        self.readRegisters() # Read the current register set
        self.si4703_registers[POWERCFG] = 0x4001 # Enable the IC
        self.si4703_registers[SYSCONFIG1] |= (1 << RDS) # Enable RDS

        if IN_EUROPE:
            self.si4703_registers[SYSCONFIG1] |= (1 << DE) # 50kHz Europe setup
            self.si4703_registers[SYSCONFIG2] |= (1 << SPACE0) # 100kHz channel spacing for Europe
        else:
            self.si4703_registers[SYSCONFIG2] &= ~(1 << SPACE1 | 1 << SPACE0)  # Force 200kHz channel spacing for USA

        self.si4703_registers[SYSCONFIG2] &= 0xFFF0 # Clear volume bits
        self.si4703_registers[SYSCONFIG2] |= 0x0001 # Set volume to lowest
        self.updateRegisters() # Update

        time.sleep(0.11) # Max powerup time, from datasheet page 13


    def readRegisters(self):
        # Anytime you READ the regsters it writes the command byte to the first byte of the
        # SI4703 0x02 register. And then uses the list to write the 2nd bytes of register 0x02
        # and continues until all the list is written!
        # Si4703 begins reading from register upper register of 0x0A and reads to 0x0F, then loops to 0x00.
        # We want to read the entire register set from 0x0A to 0x09 = 32 bytes
        i = 0
        initialReadings = self._i2c.read_i2c_block_data(self.i2c_addr, self.si4703_registers[POWERCFG] >> 8, 32)

        # Remember, register 0x0A comes in first so we have to shuffle the array around a bit
        x = 0x0A
        while 1: # Read in these 32 bytes
            if x == 0x10:
                x = 0 # Loop back to zero
            self.si4703_registers[x] = initialReadings[i] << 8
            i = i + 1
            self.si4703_registers[x] |= initialReadings[i]
            i = i + 1
            if x == 0x09:
                break # We're done!
            x = x + 1


    #  Reads the current channel from READCHAN
    #  Returns a number like 973 for 97.3MHz
    def readChannel(self):
        self.readRegisters()
        channel = self.si4703_registers[READCHAN] & 0x03FF #  Mask out everything but the lower 10 bits

        if IN_EUROPE:
            # Freq(MHz) = 0.100(in Europe) * Channel + 87.5MHz
            # X = 0.1 * Chan + 87.5
            channel *= 1 # 98 * 1 = 98 - I know this line is silly, but it makes the code look uniform
        else:
            # Freq(MHz) = 0.200(in USA) * Channel + 87.5MHz
            # X = 0.2 * Chan + 87.5
            channel *= 2 # 49 * 2 = 98

        channel += 875 # 98 + 875 = 973
        return channel


    def checkRDS(self):
        print("\nCheck RDS")
        for i in range(200):
            self.readRegisters()
            if self.si4703_registers[STATUSRSSI] & (1 << RDSR):
                blockerrors = (self.si4703_registers[STATUSRSSI] & 0x0600) >> 9 #  Mask in BLERA
                if blockerrors == 1:
                    print(" (1-2 RDS errors)")
                elif blockerrors == 2:
                    print(" (3-5 RDS errors)")
                elif blockerrors == 3:
                    print(" (6+ RDS errors)")

                Ah = (self.si4703_registers[RDSA] & 0xFF00) >> 8
                Al = (self.si4703_registers[RDSA] & 0x00FF)

                Bh = (self.si4703_registers[RDSB] & 0xFF00) >> 8
                Bl = (self.si4703_registers[RDSB] & 0x00FF)

                Ch = (self.si4703_registers[RDSC] & 0xFF00) >> 8
                Cl = (self.si4703_registers[RDSC] & 0x00FF)

                Dh = (self.si4703_registers[RDSD] & 0xFF00) >> 8
                Dl = (self.si4703_registers[RDSD] & 0x00FF)

                # print("RDS: " << str(Bh) << str(Bl) << str(Ch) << str(Cl) << str(Dh) << str(Dl))
                time.sleep(0.4)
            else:
                time.sleep(0.3)


    #  Write the current 6 control registers (0x02 to 0x07) to the Si4703
    #  It's a little weird, you don't write an I2C address
    #  The Si4703 assumes you are writing to 0x02 first, then increments
    def updateRegisters(self):
        dataToSend = 12 * [0]
        i = 0
        #  A write command automatically begins with register 0x02 so no need to send a write-to address
        #  First we send the 0x02 to 0x07 control registers
        #  In general, we should not write to registers 0x08 and 0x09
        for regSpot in range(0x02, 0x08):
            high_byte = self.si4703_registers[regSpot] >> 8
            low_byte = self.si4703_registers[regSpot] & 0x00FF

            dataToSend[i] = high_byte
            i = i + 1
            dataToSend[i] = low_byte
            i = i + 1


        print("Update registers: dataToSend = ")
        for data in dataToSend:
            print(data)
        print("\n")

        self._i2c.write_i2c_block_data(self.i2c_addr, dataToSend[0], dataToSend[1:])


    def setVolume(self, value=10):
        print("\nSet volume to value ", value)
        if value > 15:
            print("Cannot set volume. Must be between [0, 15]")
            return

        self.readRegisters()
        self.si4703_registers[SYSCONFIG2] &= 0xFFF0 #  Clear volume bits
        self.si4703_registers[SYSCONFIG2] |= value
        self.updateRegisters() #  Update


    #  Seeks out the next available station
    #  Returns the freq if it made it
    #  Returns zero if failed
    def seek(self, seekDirection):
        print("\n Seek")
        self.readRegisters()

        # Set seek mode wrap bit
        self.si4703_registers[POWERCFG] |= (1 << SKMODE) # Allow wrap
        #     self.si4703_registers[POWERCFG] &= ~(1 << SKMODE) # Disallow wrap - if you disallow wrap, you may want to tune to 87.5 first

        if seekDirection == SEEK_DOWN:
            self.si4703_registers[POWERCFG] &= ~(1 << SEEKUP) # Seek down is the default upon reset
        else:
            self.si4703_registers[POWERCFG] |= 1 << SEEKUP # Set the bit to seek up

        self.si4703_registers[POWERCFG] |= (1 << SEEK) # Start seek

        self.updateRegisters() # Seeking will now start

        # Poll to see if STC is set
        while 1:
            self.readRegisters()
            if (self.si4703_registers[STATUSRSSI] & (1 << STC)) != 0:
                break # Tuning complete!

        print("Trying station: ", self.readChannel())

        self.readRegisters()
        valueSFBL = self.si4703_registers[STATUSRSSI] & (1 << SFBL) # Store the value of SFBL
        self.si4703_registers[POWERCFG] &= ~(1 << SEEK) # Clear the seek bit after seek has completed
        self.updateRegisters()

        # Wait for the si4703 to clear the STC as well
        while 1:
            self.readRegisters()
            if (self.si4703_registers[STATUSRSSI] & (1 << STC)) == 0:
                break # Tuning complete!
            print("Waiting...")

        if valueSFBL:  # The bit was set indicating we hit a band limit or failed to find a station
            print("Seek limit hit") # Hit limit of band during seek
            return False

        print("Channel set to ", self.readChannel())
        return True


    #  Given a channel, tune to it
    #  Channel is in MHz, so 973 will tune to 97.3MHz
    #  Note: gotoChannel will go to illegal channels (ie, greater than 110MHz)
    #  It's left to the user to limit these if necessary
    #  Actually, during testing the Si4703 seems to be internally limiting it at 87.5. Neat.
    def goToChannel(self, value = 999):
        print("\nGo to channel ", value)
        #  write channel reg 03h
        newChannel = value
        newChannel *= 10
        newChannel -= 8750

        if IN_EUROPE:
            newChannel //= 10 # 980 / 10 = 98
        else:
            newChannel //= 20 # 980 / 20 = 49

        #  These steps come from AN230 page 20 rev 0.5
        self.readRegisters()
        self.si4703_registers[CHANNEL] &= 0xFE00 #  Clear out the channel bits
        self.si4703_registers[CHANNEL] |= newChannel #  Mask in the new channel
        self.si4703_registers[CHANNEL] |= (1 << TUNE) #  Set the TUNE bit to start
        self.updateRegisters()

        #  Poll to see if STC is set
        while 1:
            time.sleep(0.5)
            self.readRegisters()
            if (self.si4703_registers[STATUSRSSI] & (1 << STC)) != 0:
                break #  Tuning complete!
            print("Tuning...")

        self.readRegisters()
        self.si4703_registers[CHANNEL] &= ~(1 << TUNE) #  Clear the tune after a tune has completed
        self.updateRegisters()

        #  Wait for the si4703 to clear the STC as well
        while 1:
            self.readRegisters()
            if (self.si4703_registers[STATUSRSSI] & (1 << STC)) == 0:
                break # Tuning complete!
            print("Waiting...")

        print("Station set to ", self.readChannel())

