IN_EUROPE = 1

# Direction used for seeking. Default is down
SEEK_DOWN = 0
SEEK_UP = 1

# Define the register names
DEVICEID = 0x00
CHIPID = 0x01
POWERCFG = 0x02
CHANNEL = 0x03
SYSCONFIG1 = 0x04
SYSCONFIG2 = 0x05
STATUSRSSI = 0x0A
READCHAN = 0x0B
RDSA = 0x0C
RDSB = 0x0D
RDSC = 0x0E
RDSD = 0x0F

# Register 0x02 - POWERCFG
SMUTE = 15
DMUTE = 14
SKMODE = 10
SEEKUP = 9
SEEK = 8

# Register 0x03 - CHANNEL
TUNE = 15

# Register 0x04 - SYSCONFIG1
RDS = 12
DE = 11

# Register 0x05 - SYSCONFIG2
SPACE1 = 5
SPACE0 = 4

# Register 0x0A - STATUSRSSI
RDSR = 15
STC = 14
SFBL = 13
AFCRL = 12
RDSS = 11
STEREO = 8