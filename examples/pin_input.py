import time
from machine import u2if, Pin, Signal


def get_state_string(value):
    return "LOW" if value == Pin.LOW else "HIGH"


switch = Pin(u2if.GP_9, Pin.IN, pull=Pin.PULL_UP)

for _ in range(10):
    input_value = switch.value()
    print("switch state : %s" % get_state_string(input_value))
    time.sleep(1)

switch_invert = Signal(Pin(u2if.GP_8, Pin.IN, pull=Pin.PULL_UP), invert=True)

for _ in range(10):
    input_value = switch_invert.value()
    print("switch (inverted) state : %s" % get_state_string(input_value))
    time.sleep(1)
