from .u2if import Device
from . import u2if_const as report_const


class WS2812B:

    def __init__(self, pin_id, direction=None, pull=None, value=None):
        self._initialized = False
        self._device = Device()
        self.pin_id = pin_id
        self._initialized = self._init()

    def __del__(self):
        self.deinit()

    def _init(self):
        res = self._device.send_report(bytes([report_const.WS2812B_INIT, self.pin_id]))
        if res[1] != report_const.OK:
            raise RuntimeError("WS2812B init error.")
        return True
        pass

    def deinit(self):
        if not self._initialized:
            return
        res = self._device.send_report(bytes([report_const.WS2812B_DEINIT]))
        if res[1] != report_const.OK:
            raise RuntimeError("WS2812B deinit error.")
        pass

    def write(self, buffer):
        return self._write_stream(buffer)

    def _write_stream(self, pixel_list):
        self._device.reset_output_serial()
        buffer = []
        # Not optimized version because buffer it is organized for PIO fifo and not for the transmission speed
        for pixel in pixel_list:
            # in uint32_t LSB order
            buffer.append(0)
            buffer.append(pixel[2] & 0xFF)
            buffer.append(pixel[0] & 0xFF)
            buffer.append(pixel[1] & 0xFF)


        remain_bytes = len(buffer)
        res = self._device.send_report(bytes([report_const.WS2812B_WRITE]) + remain_bytes.to_bytes(4, byteorder='little'))
        if res[1] != report_const.OK and res[2] == 0x01:
            raise RuntimeError("WS2812B write error : too many pixel for the firmware.")
        elif res[1] != report_const.OK and res[2] == 0x02:
            raise RuntimeError("WS2812B write error: transfer already in progress.")
        elif res[1] != report_const.OK:
            raise RuntimeError("WS2812B write error")

        self._device.write_serial(buffer)
        res = self._device.read_hid(report_const.WS2812B_WRITE)
        if res[1] != report_const.OK:
            raise RuntimeError("WS2812B write error.")
