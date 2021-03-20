import time
from machine import u2if, Pin, Signal

# Initialize GPIO to output and set the value HIGH
led = Pin(u2if.GP_3, Pin.OUT, value=Pin.HIGH)
time.sleep(1)
# Switch off the led
led.value(Pin.LOW)


# Active-low RGB led
led_r = Signal(Pin(u2if.GP_6, Pin.OUT), invert=True)
led_g = Signal(Pin(u2if.GP_7, Pin.OUT), invert=True)
led_b = Signal(Pin(u2if.GP_8, Pin.OUT), invert=True)

# Switch on the three colors
led_r.value(Pin.HIGH)
led_g.on()  # == .value(Pin.HIGH)
led_b.on()
time.sleep(1)
# Switch off the three colors
led_r.off()
led_g.value(Pin.LOW)  # == .value(Pin.LOW)
led_b.value(Pin.LOW)
