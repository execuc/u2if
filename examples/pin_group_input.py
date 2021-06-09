import time
from machine import u2if, Pin, GroupPin

# Simple test of grouped input


def get_state_string(value):
    return "LOW" if value == Pin.LOW else "HIGH"


group_input = GroupPin([Pin(u2if.GP9, Pin.IN, pull=Pin.PULL_UP),
                        Pin(u2if.GP8, Pin.IN, pull=Pin.PULL_UP)])

for _ in range(10):
    input_value = group_input.value()
    input_not_value = group_input.not_value()
    print("group input value: %s (~: %s)" % (input_value, input_not_value))
    time.sleep(1)


