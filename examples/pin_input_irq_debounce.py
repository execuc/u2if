import time
from machine import u2if, Pin


def irq_callback(pin, event=None):
    is_rising = event & Pin.IRQ_RISING
    is_falling = event & Pin.IRQ_FALLING
    info_interrupt = '%s pin %d =>' % (time.time()*1000, pin)
    if is_rising:
        info_interrupt += ' RISING'
    if is_falling:
        info_interrupt += ' FALLING'
    print(info_interrupt)


# Configure first switch
switch = Pin(u2if.GP_9, Pin.IN, pull=Pin.PULL_UP)
switch.irq(handler=irq_callback, trigger=Pin.IRQ_RISING | Pin.IRQ_FALLING)
# Configure second switch with debouncer
switch_debounced = Pin(u2if.GP_8, Pin.IN, pull=Pin.PULL_UP)
switch_debounced.irq(handler=irq_callback, trigger=Pin.IRQ_FALLING, debounce=True)

while True:
    # Retrieve all irq recorded and call handler
    Pin.process_irq()
    time.sleep(0.005)

