### NodeMCU I2C communication with BME680 using MicroPython

This is a basic communication between NodeMCU and BME680 inspired from https://github.com/robmarkcole/bme680-mqtt-micropython.
Don't forget to set your SSID credentials in boot.py. You also need to use `mpy-cross` in order to compile the `*.py` files in bme680 directory (otherwise you get `MemoryError: memory allocation failed`).
