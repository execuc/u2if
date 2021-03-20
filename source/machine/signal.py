from .pin import Pin


class Signal:

    def __init__(self, pin, invert=False):
        self.pin = pin
        self.invert = invert

    def _adjust_value(self, value):
        if self.invert and value == Pin.HIGH:
            return Pin.LOW
        elif self.invert and value == Pin.LOW:
            return Pin.HIGH
        else:
            return value

    def value(self, value_to_set=None):
        if value_to_set is None:
            return self._adjust_value(self.pin.value())
        else:
            return self.pin.value(self._adjust_value(value_to_set))

    def on(self):
        self.value(Pin.HIGH)

    def off(self):
        self.value(Pin.LOW)

