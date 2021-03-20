# Example and BMP280 driver derived from https://github.com/adafruit/Adafruit_CircuitPython_BMP280 which is licensed:
# SPDX-FileCopyrightText: 2021 ladyada for Adafruit Industries
# # SPDX-License-Identifier: MIT

import time
from external.bmp280 import *
from machine import I2C, SPI, u2if, Pin


# Connect a BMP280 in I2C (index 0) :
# VCC: 3.3v
# GND: GND
# SCL: I2C0_SCL
# SDA: I2C0_SDA
#i2c = I2C(i2c_index=0, frequency=400000) # , pullup=True
#bmp280 = Adafruit_BMP280_I2C(i2c, 0x76)

# or in SPI:
# VCC: 3.3v
# GND: GND
# SCL: SPI0_CLK
# SDA: SPI0_MOSI (=SDI)
# CSB: SPI0_CS1
# SDO: SPI0_MISO
spi = SPI(spi_index=0)
spi.init(baudrate=10000000)
cs = Pin(u2if.SPI0_CS1, Pin.OUT, value=Pin.HIGH)
bmp280 = Adafruit_BMP280_SPI(spi, cs)



# change this to match the location's pressure (hPa) at sea level
bmp280.sea_level_pressure = 1013.25
bmp280.mode = MODE_NORMAL
bmp280.standby_period = STANDBY_TC_500
bmp280.iir_filter = IIR_FILTER_X16
bmp280.overscan_pressure = OVERSCAN_X16
bmp280.overscan_temperature = OVERSCAN_X2
# The sensor will need a moment to gather inital readings
time.sleep(1)

while True:
    print("\nTemperature: %0.1f C" % bmp280.temperature)
    print("Pressure: %0.1f hPa" % bmp280.pressure)
    print("Altitude = %0.2f meters" % bmp280.altitude)
    time.sleep(1)


