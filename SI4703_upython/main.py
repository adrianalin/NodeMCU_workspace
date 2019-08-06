from si4703 import SI4703


FM_receiver = SI4703(i2c_addr=0x10)
while 1:
    val = int(input("Seek: "))
    FM_receiver.seek(val)