import time
from external.rotary import Encoder
from machine import Pin, u2if

# This example uses a rotary encore from www.diymore.cc.
# S1 and S2 are debounced in hardware
# S1, S2 and Key are pulled-high.
# GND: GND
# Vcc : 3.3v
# S1: GP21
# S2: GP22
# Key: GP28


def irq_callback(pin, event=None):
    if pin == u2if.GP28 and event == Pin.IRQ_FALLING:
        print("Key pressed")


s1 = Pin(u2if.GP21, Pin.IN)
s2 = Pin(u2if.GP22, Pin.IN)

encoder = Encoder(s1, s2)
key = Pin(u2if.GP28, Pin.IN)
key.irq(handler=irq_callback, trigger=Pin.IRQ_FALLING, debounce=True)  # software debounced in u2if

last_value = encoder.read()
while True:
    # Retrieve all irq recorded and call handler
    Pin.process_irq()
    value = encoder.read()
    if last_value != value:
        print("Encoder value: %d" % value)
        last_value = value
    time.sleep(0.005)
