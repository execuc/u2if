from .u2if import Device
from . import u2if_const as report_const


class SPI(object):
    def __init__(self, *, spi_index=0):
        self.spi_index = spi_index
        self._initialized = False
        self._device = Device()

    def __del__(self):
        self.deinit()

    def init(self, baudrate=1000000):
        report_id = report_const.SPI0_INIT if self.spi_index == 0 else report_const.SPI1_INIT
        mode = 0x00  # to implement
        res = self._device.send_report(bytes([report_id, mode]) + baudrate.to_bytes(4, byteorder='little'))
        if res[1] != report_const.OK:
            raise RuntimeError("SPI init error.")
        self._initialized = True

    def deinit(self):
        if not self._initialized:
            return
        report_id = report_const.SPI0_DEINIT if self.spi_index == 0 else report_const.SPI1_DEINIT
        res = self._device.send_report(bytes([report_id]))
        if res[1] != report_const.OK:
            raise RuntimeError("SPI deinit error.")

    def read(self, nbytes, write=0):
        buf = bytearray(nbytes)
        self.readfrom_into(buf, write)
        return buf

    def readinto(self, buf, write=0):
        return self._read_from_into(buf, write)

    def write(self, buf):
        return self._spi_write(buf)

    def write_readinto(self, write_buf, read_buf):
        raise RuntimeError('Not implemented')

    def _read_from_into(self, buf, write_byte=0):
        read_size = len(buf)
        report_id = report_const.SPI0_READ if self.spi_index == 0 else report_const.SPI1_READ
        res = self._device.send_report(bytes([report_id, write_byte, read_size]))
        if res[1] != report_const.OK:
            raise RuntimeError("SPI read error.")
        for i in range(read_size):
            buf[i] = res[i+2]

    def _spi_write(self, buf):
        if len(buf) > 3 * report_const.HID_REPORT_SIZE:
            self._spi_write_stream(buf)
        else:
            self._spi_write_direct(buf)

    def _spi_write_direct(self, buf):
        report_id = report_const.SPI0_WRITE if self.spi_index == 0 else report_const.SPI1_WRITE
        start = 0
        end = len(buf)
        while (end - start) > 0:
            remain_bytes = end - start
            chunk = min(remain_bytes, report_const.HID_REPORT_SIZE - 3)
            res = self._device.send_report(bytes([report_id, chunk]) + bytes(buf[start: (start + chunk)]))
            if res[1] != report_const.OK:
                raise RuntimeError("SPI write direct error.")
            start += chunk

    def _spi_write_stream(self, buf):
        self._device.reset_output_serial()
        report_id = report_const.SPI0_WRITE_FROM_UART if self.spi_index == 0 else report_const.SPI1_WRITE_FROM_UART
        remain_bytes = len(buf)
        res = self._device.send_report(bytes([report_id]) + remain_bytes.to_bytes(4, byteorder='little'))
        if res[1] != report_const.OK:
            raise RuntimeError("SPI write error.")

        self._device.write_serial(buf)
        res = self._device.read_hid(report_id)
        if res[1] != report_const.OK:
            raise RuntimeError("SPI write error.")

