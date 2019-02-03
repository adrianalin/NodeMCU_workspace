I2C Communication between NodeMcu and BME680:

1. download the Bosch library from https://www.bosch-sensortec.com/bst/products/all_products/bsec

2. extract the downloaded zip BSEC_1.4.7.1_Generic_Release_20180907.zip

3. import the Arduino library BSEC_1.4.7.1_Generic_Release_20180907/Arduino/BSEC.zip
in Arduino IDE (i used version 1.8.8 )

4. copy the closed source lib BSEC_1.4.7.1_Generic_Release_20180907/algo/bin/Normal_version/esp8266/libalgobsec.a to ~/Arduino/libraries/bsec/src/esp8266

5. follow the instructions about the linker from the https://github.com/BoschSensortec/BSEC-Arduino-library

Wires connection between NodeMcu and bme680:

BME680    ESP8266 version 12E
GND------GND
3V3------3V3
SCL------D1
SDA------D2
