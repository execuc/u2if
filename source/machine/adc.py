from .u2if import Device
from . import u2if_const as report_const


class ADC:
    def __init__(self, pin_id):
        self.pin_id = pin_id
        self._device = Device()
        self._init()

    def _init(self):
        res = self._device.send_report(bytes([report_const.ADC_INIT_PIN, self.pin_id]))
        if res[1] != report_const.OK:
            raise RuntimeError("ADC init error.")

    def value(self):
        res = self._device.send_report(bytes([report_const.ADC_GET_VALUE, self.pin_id]))
        if res[1] != report_const.OK:
            raise RuntimeError("ADC read error.")
        return int.from_bytes(res[3:3+2], byteorder='little')
