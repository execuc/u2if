# Example and vl53l0x driver derived from https://raw.githubusercontent.com/adafruit/Adafruit_CircuitPython_VL53L0X which is licensed:
# SPDX-FileCopyrightText: 2021 ladyada for Adafruit Industries
# SPDX-License-Identifier: MIT

# Simple demo of the VL53L0X distance sensor.
# Will print the sensed range/distance every second.
import time

from external import adafruit_vl53l0x
from machine import I2C

# Initialize I2C bus and sensor. Use first I2C port.
i2c = I2C(i2c_index=0, frequency=400000) # , pullup=True
vl53 = adafruit_vl53l0x.VL53L0X(i2c)


# Optionally adjust the measurement timing budget to change speed and accuracy.
# See the example here for more details:
#   https://github.com/pololu/vl53l0x-arduino/blob/master/examples/Single/Single.ino
# For example a higher speed but less accurate timing budget of 20ms:
# vl53.measurement_timing_budget = 20000
# Or a slower but more accurate timing budget of 200ms:
# vl53.measurement_timing_budget = 200000
# The default timing budget is 33ms, a good compromise of speed and accuracy.

# Main loop will read the range and print it every second.
while True:
    print("Range: {0}mm".format(vl53.range))
    time.sleep(1.0)
