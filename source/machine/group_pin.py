from .u2if import Device
from . import Pin
from . import u2if_const as report_const


class GroupPin:

    def __init__(self, pins_list,value=None):
        self._pins = pins_list
        self._device = Device()
        self._mask = 0x00
        self.init(value)

    def __del__(self):
        pass

    def init(self, value=None):
        group_dir = self._pins[0].mode
        if group_dir not in [Pin.IN, Pin.OUT]:
            raise ValueError('Group pins have to be Pin.IN or Pin.OUT')

        self._mask = 0x00
        for pin in self._pins:
            if pin.mode != group_dir:
                raise ValueError('Pin direction inconsistency direction in group')
            self._mask |= 1 << pin.id

        # dirs = 0
        # pull = 0
        #
        # res = self._device.send_report(bytes([report_const.MULTI_GPIO_INIT]) +
        #                                      self._mask.to_bytes(4, byteorder='little') +
        #                                      dirs.to_bytes(4, byteorder='little') + pull.to_bytes(4, byteorder='little'))
        # if res[1] != report_const.OK:
        #     raise RuntimeError("Multi pins init error.")
        #
        # if value is not None:
        #     self.value(value)

    # def on(self):
    #     return self.value(self.HIGH)
    #
    # def off(self):
    #     return self.value(self.LOW)

    def value(self, value_to_set=None):
        if value_to_set is None:
            return self._get_value()
        else:
            return self._set_value(value_to_set)

    def not_value(self):
        return self._get_value(invert=True)

    def _get_value(self, invert=False):
        res = self._device.send_report(bytes([report_const.GROUP_GPIO_GET_ALL_VALUES]))
        if res[1] != report_const.OK:
             raise RuntimeError("Pin read error.")
        input = int.from_bytes(res[2:2+4], byteorder='little')

        value = 0
        bit_index = 0
        for pin in reversed(self._pins):
            bit = ((input >> pin.id) & 0x01)
            if invert is True:
                bit =  not bit
            value |= bit << bit_index
            bit_index = bit_index + 1
        return value

    def _set_value(self, value):
        pin_values = 0
        bit_index = 0b1
        for pin in reversed(self._pins):
            if value & bit_index > 0:
                pin_values |= 1 << pin.id
            bit_index = bit_index << 1

        res = self._device.send_report(bytes([report_const.GROUP_GPIO_SET_VALUES]) +
                                              self._mask.to_bytes(4, byteorder='little') +
                                              pin_values.to_bytes(4, byteorder='little'))
        if res[1] != report_const.OK:
            raise RuntimeError("Groups pins write error.")
        return value
