import gc
import bme680
from machine import Pin, I2C
from bme680.i2c import I2CAdapter
import time

# construct an I2C bus
i2c_dev = I2CAdapter(scl=Pin(5), sda=Pin(4), freq=100000)
i2c_dev.scan()
sensor = bme680.BME680(i2c_device=i2c_dev)
# These oversampling settings can be tweaked to
# change the balance between accuracy and noise in
# the data.
sensor.set_humidity_oversample(bme680.OS_2X)
sensor.set_pressure_oversample(bme680.OS_4X)
sensor.set_temperature_oversample(bme680.OS_8X)
sensor.set_filter(bme680.FILTER_SIZE_3)

print("Polling:")
try:
    while True:
        if sensor.get_sensor_data():

            output = "{} C, {} hPa, {} RH, {} RES,".format(
                sensor.data.temperature,
                sensor.data.pressure,
                sensor.data.humidity,
                sensor.data.gas_resistance)

            print(output)
            time.sleep(1)
except KeyboardInterrupt:
    pass