from .u2if import Device
from . import u2if_const as report_const


class Pin:
    IN = 0
    OUT = 1
    LOW = 0
    HIGH = 1
    PULL_NONE = 0
    PULL_UP = 1
    PULL_DOWN = 2
    IRQ_FALLING = report_const.EVENT_FALLING
    IRQ_RISING = report_const.EVENT_RISING

    def __init__(self, pin_id, mode=None, pull=None, value=None):
        self.id = pin_id
        self.mode = mode
        self.has_irq = False
        self._device = Device()
        if mode is not None:
            self.init(mode, pull, value)

    def __del__(self):
        self._remove_irq()

    @staticmethod
    def process_irq():
        Device().process_irq()

    def init(self, mode, pull=None, value=None):
        config_pull = 0x00
        if mode == self.IN and pull == self.PULL_UP:
            config_pull = 0x01
        elif mode == self.IN and pull == self.PULL_DOWN:
            config_pull = 0x02
        direction_conf = 0x00 if mode == self.IN else 0x01
        res = self._device.send_report(bytes([report_const.GPIO_INIT_PIN, self.id, direction_conf, config_pull]))
        if res[1] != report_const.OK:
            raise RuntimeError("Pin init error.")

        if value is not None:
            self.value(value)

    def on(self):
        return self.value(self.HIGH)

    def off(self):
        return self.value(self.LOW)

    def value(self, value_to_set=None):
        if value_to_set is None:
            return self._get_value()
        else:
            return self._set_value(value_to_set)

    def _get_value(self):
        res = self._device.send_report(bytes([report_const.GPIO_GET_VALUE, self.id]))
        if res[1] != report_const.OK:
            raise RuntimeError("Pin read error.")
        return self.LOW if res[3] == 0x00 else self.HIGH

    def _set_value(self, value):
        value_cmd = 0x00 if value == self.LOW else 0x01
        res = self._device.send_report(bytes([report_const.GPIO_SET_VALUE, self.id, value_cmd]))
        if res[1] != report_const.OK:
            raise RuntimeError("Pin read error.")
        return value

    def irq(self, handler=None, trigger=IRQ_FALLING | IRQ_RISING, debounce=False):
        if handler is None or trigger == report_const.EVENT_NONE:
            return self._remove_irq()
        else:
            return self._add_irq(handler, trigger, debounce)

    def _remove_irq(self):
        if self.has_irq is False:
            return

        self._device.unregister_callback(self.id)
        res = self._device.send_report(bytes([report_const.GPIO_SET_IRQ, self.id, report_const.EVENT_NONE, 0x00]))
        if res[1] != report_const.OK:
            raise RuntimeError("Remove irq error.")
        self.has_irq = False

    def _add_irq(self, callback, events, debounce=False):
        debounce_flag = 0x00 if debounce is False else 0x01
        res = self._device.send_report(bytes([report_const.GPIO_SET_IRQ, self.id, events, debounce_flag]))
        if res[1] != report_const.OK:
            raise RuntimeError("Remove irq error.")
        self._device.register_callback(self.id, callback)
        self.has_irq = True
