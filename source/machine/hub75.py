import queue
import time
from .u2if import Device
from . import u2if_const as report_const


class HUB75:
    def __init__(self, width, height):
        self._initialized = False
        self._width = width
        self._height = height
        self._device = Device()
        self.init()

    def __del__(self):
        self.deinit()

    def init(self):
        res = self._device.send_report(bytes([report_const.HUB75_INIT, self._width, self._height]))
        if res[1] != report_const.OK:
            raise RuntimeError("Hub75 init error.")
        self._initialized = True
        return True

    def deinit(self):
        if not self._initialized:
            return
        res = self._device.send_report(bytes([report_const.HUB75_DEINIT]))
        if res[1] != report_const.OK:
            raise RuntimeError("Hub75 deinit error.")

    def write(self, buffer):
        return self._write_stream(buffer)

    def _write_stream(self, pixel_list):
        self._device.reset_output_serial()
        buffer = []
        # Not optimized version because buffer it is organized for PIO fifo and not for the transmission speed
        for pixel in pixel_list:
            # in uint32_t LSB order
            buffer.append(pixel[0] & 0xFF)
            buffer.append(pixel[1] & 0xFF)
            buffer.append(pixel[2] & 0xFF)
            buffer.append(0)

        remain_bytes = len(buffer)
        res = self._device.send_report(bytes([report_const.HUB75_WRITE]) + remain_bytes.to_bytes(4, byteorder='little'))
        if res[1] != report_const.OK and res[2] == 0x01:
            raise RuntimeError("HUB75 write error : too many pixel for the firmware.")
        elif res[1] != report_const.OK and res[2] == 0x02:
            raise RuntimeError("HUB75 write error: transfer already in progress.")
        elif res[1] != report_const.OK:
            raise RuntimeError("HUB75 write error")

        self._device.write_serial(buffer)
        res = self._device.read_hid(report_const.HUB75_WRITE)
        if res[1] != report_const.OK:
            raise RuntimeError("HUB75 write error.")
