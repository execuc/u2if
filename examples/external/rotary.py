# From https://github.com/mivallion/Encoder which is originally licensed : see below
# Somes changes to work on u2if module. Mainly to have the input states according to their
# event (rising & falling). Indeed, with the u2if module, the code is not executed
# on the microcontroller but in a remote way.
# ********************************************************************************************************

"""
Encoder library for Raspberry Pi for measuring quadrature encoded signals.
created by Mivallion <mivallion@gmail.com>
Version 1.0 - 01 april 2020 - inital release
"""

#import RPi.GPIO as GPIO
from machine import Pin


class Encoder(object):
    """
    Encoder class allows to work with rotary encoder
    which connected via two pin A and B.
    Works only on interrupts because all RPi pins allow that.
    This library is a simple port of the Arduino Encoder library
    (https://github.com/PaulStoffregen/Encoder)
    """
    def __init__(self, A, B):
        #GPIO.setmode(GPIO.BCM)
        #GPIO.setup(A, GPIO.IN)
        #GPIO.setup(B, GPIO.IN)
        self.pinA = A
        self.pinB = B

        self.pinA_value = self.pinA.value()
        self.pinB_value = self.pinB.value()

        self.pos = 0
        self.state = 0
        if self.pinA_value:
            self.state |= 1
        if self.pinB_value:
            self.state |= 2

        #GPIO.add_event_detect(A, GPIO.BOTH, callback=self.__update)
        #GPIO.add_event_detect(B, GPIO.BOTH, callback=self.__update)
        self.pinA.irq(handler=self.__update, trigger=Pin.IRQ_FALLING | Pin.IRQ_RISING)
        self.pinB.irq(handler=self.__update, trigger=Pin.IRQ_FALLING | Pin.IRQ_RISING)

    # Get update state from their event. Do not get state from pin.value() because with the interruptions latency,
    # they will no longer have the right value
    def _update_input_value(self, pin, event):
        is_rising = event & Pin.IRQ_RISING
        is_falling = event & Pin.IRQ_FALLING

        if event == 0x03:
            return

        if pin == self.pinA.id:
            if self.pinA_value == Pin.HIGH and is_falling:
                self.pinA_value = Pin.LOW
            elif self.pinA_value == Pin.LOW and is_rising:
                self.pinA_value = Pin.HIGH
        elif pin == self.pinB.id :
            if self.pinB_value == Pin.HIGH and is_falling:
                self.pinB_value = Pin.LOW
            elif self.pinB_value == Pin.LOW and is_rising:
                self.pinB_value = Pin.HIGH


    """
    update() calling every time when value on A or B pins changes.
    It updates the current position based on previous and current states
    of the rotary encoder.
    """
    def __update(self, pin, event):
        self._update_input_value(pin, event)

        state = self.state & 3
        if self.pinA_value:
            state |= 4
        if self.pinB_value:
            state |= 8

        self.state = state >> 2

        if state == 1 or state == 7 or state == 8 or state == 14:
            self.pos += 1
        elif state == 2 or state == 4 or state == 11 or state == 13:
            self.pos -= 1
        elif state == 3 or state == 12:
            self.pos += 2
        elif state == 6 or state == 9:
            self.pos -= 2


    """
    read() simply returns the current position of the rotary encoder.
    """
    def read(self):
        return self.pos
