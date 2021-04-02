from .u2if import Device
from . import u2if_const as report_const


class I2S:

    def __init__(self):
        self._initialized = False
        self._device = Device()
        self._initialized = False
        self._init()

    def __del__(self):
        self.deinit()

    def _init(self):
        if self._initialized:
            return
        res = self._device.send_report(bytes([report_const.I2S_INIT]))
        if res[1] != report_const.OK:
            raise RuntimeError("I2S init error.")
        self._initialized = True

    def deinit(self):
        if not self._initialized:
            return
        res = self._device.send_report(bytes([report_const.I2S_DEINIT]))
        if res[1] != report_const.OK:
            raise RuntimeError("I2S deinit error.")
        self._initialized = False

    def set_freq(self, frequency):
        res = self._device.send_report(bytes([report_const.I2S_SET_FREQ]) + frequency.to_bytes(4, byteorder='little'))
        if res[1] != report_const.OK:
            raise RuntimeError("I2S set freq error.")
        return True

    def write(self, buffer):
        self._device.reset_output_serial()
        remain_bytes = len(buffer)
        res = self._device.send_report(bytes([report_const.I2S_WRITE_BUFFER]) + remain_bytes.to_bytes(4, byteorder='little'))
        if res[1] != report_const.OK and res[2] == 0x01:
            raise RuntimeError("I2S write error : too many bytes")
        elif res[1] != report_const.OK and res[2] == 0x02:
            raise RuntimeError("I2S write error: no buffer available.")
        elif res[1] != report_const.OK:
            raise RuntimeError("I2S write error")

        self._device.write_serial(buffer)
        res = self._device.read_hid(report_const.I2S_WRITE_BUFFER)
        if res[1] != report_const.OK:
            raise RuntimeError("I2S write error.")
