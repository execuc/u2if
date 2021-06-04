# Example and vl53l0x driver derived from https://raw.githubusercontent.com/adafruit/Adafruit_CircuitPython_VL53L0X which is licensed:
# SPDX-FileCopyrightText: 2021 ladyada for Adafruit Industries
# SPDX-License-Identifier: MIT

import time

from external import adafruit_vl53l0x
from machine import I2C, Pin, u2if

# Demo of the VL53L0X distance sensor with interrupt.
# There are two mode:
# * Manual: user set low GP9 (button with pull up) => trigger a measure
# * Auto: read measure as fast as it can.
# Module interrupt has to be wired to GP13 and button if needed, to GP9

mode_auto = False   # Set False to manual trigger with GP9
start_measure = True


def irq_callback(pin, event=None):
    global start_measure
    is_falling = event & Pin.IRQ_FALLING
    if pin == u2if.GP13 and is_falling:
        print("Range: {0}mm".format(vl53.read_measure()))
        start_measure = mode_auto
    elif pin == u2if.GP9 and is_falling:
        start_measure = True


# Initialize I2C bus and sensor. Use first I2C port.
i2c = I2C(i2c_index=0, frequency=400000) # , pullup=True
int_pin = Pin(u2if.GP13, Pin.IN)
int_pin.irq(handler=irq_callback, trigger=Pin.IRQ_RISING | Pin.IRQ_FALLING)
button_pin = Pin(u2if.GP9, Pin.IN)
button_pin.irq(handler=irq_callback, trigger=Pin.IRQ_FALLING, debounce=True)
vl53 = adafruit_vl53l0x.VL53L0X(i2c)


# Optionally adjust the measurement timing budget to change speed and accuracy.
# See the example here for more details:
#   https://github.com/pololu/vl53l0x-arduino/blob/master/examples/Single/Single.ino
# For example a higher speed but less accurate timing budget of 20ms:
# vl53.measurement_timing_budget = 20000
# Or a slower but more accurate timing budget of 200ms:
# vl53.measurement_timing_budget = 200000
# The default timing budget is 33ms, a good compromise of speed and accuracy.

vl53.measurement_timing_budget = 100000

# Main loop will read the range and print it every second.
while True:
    if start_measure:
        vl53.start_measure()
        start_measure = False

    Pin.process_irq()
    time.sleep(0.005)
