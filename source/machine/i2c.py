from .u2if import Device
from . import u2if_const as report_const

# TODO: deinit


class I2C(object):
    def __init__(self, *, i2c_index=0, frequency=100000, pullup=False):
        self.i2c_index = i2c_index
        self._initialized = False
        self._device = Device()
        self._i2c_configure(frequency, pullup)

    def __del__(self):
        self.deinit()

    def deinit(self):
        if not self._initialized:
            return
        res = self._device.send_report(bytes([report_const.I2C0_DEINIT if self.i2c_index == 0 else report_const.I2C1_DEINIT]))
        if res[1] != report_const.OK:
            raise RuntimeError("I2c deinit error.")

    # MicroPython I2C methods
    def scan(self):
        return self._i2c_scan()

    def readfrom(self, addr, nbytes, stop=True):
        buf = bytearray(nbytes)
        self.readfrom_into(addr, buf, stop)
        return buf

    def readfrom_into(self, addr, buf, stop=True):
        return self._i2c_readfrom_into(addr, buf, stop)

    def writeto(self, addr, buf, stop=True):
        return self._i2c_writeto(addr, buf, stop)

    def writevto(self, addr, vector, stop=True):
        raise RuntimeError('Not implemented')

    # MicroPython I2C convenient methods
    def readfrom_mem(self, addr, memaddr, nbytes):
        self.writeto(addr, bytes([memaddr]), False)
        return self.readfrom(addr, nbytes, True)

    def readfrom_mem_into(self, addr, memaddr, buf):
        self.writeto(addr, bytes([memaddr]), False)
        return self.readfrom_into(addr, buf, True)

    def writeto_mem(self, addr, memaddr, buf):
        return self.writeto(addr, bytes([memaddr]) + bytes(buf), False)

    # Internal methods
    def _i2c_configure(self, baudrate=100000, pullup=False) :
        res = self._device.send_report(
            bytes([report_const.I2C0_INIT if self.i2c_index == 0 else report_const.I2C1_INIT,
                   0x00 if not pullup else 0x01])
            + baudrate.to_bytes(4, byteorder='little')
        )
        if res[1] != report_const.OK:
            raise RuntimeError("I2C init error.")

    def _i2c_scan(self, start=0, end=0x79):
        found = []
        for addr in range(start, end + 1):
            try:
                self._i2c_writeto(addr, b"\x00\x00\x00")
            except RuntimeError:
                continue
            found.append(addr)
        return found

    def _i2c_readfrom_into(self, addr, buf, stop=True):
        read_size = len(buf)
        report_id = report_const.I2C0_READ if self.i2c_index == 0 else report_const.I2C1_READ
        res = self._device.send_report(bytes([report_id, addr, 0x01 if stop else 0x00, read_size]))
        if res[1] != report_const.OK:
            raise RuntimeError("I2C read error.")
        for i in range(read_size):
            buf[i] = res[i+2]

    def _i2c_writeto(self, addr, buf, stop=True):
        if stop and len(buf) > 3 * report_const.HID_REPORT_SIZE:
            self._i2c_writeto_stream(addr, buf, stop)
        else:
            self._i2c_writeto_direct(addr, buf, stop)

    def _i2c_writeto_stream(self, addr, buf, stop=True):
        if not stop:
            raise RuntimeError('_i2c_writeto_stream with not stop Not implemented')

        self._device.reset_output_serial()
        report_id = report_const.I2C0_WRITE_FROM_UART if self.i2c_index == 0 else report_const.I2C1_WRITE_FROM_UART
        remain_bytes = len(buf)
        res = self._device.send_report(bytes([report_id, addr]) + remain_bytes.to_bytes(4, byteorder='little'))
        if res[1] != report_const.OK:
            raise RuntimeError("I2C write error.")

        self._device.write_serial(buf)
        res = self._device.read_hid(report_id)
        if res[1] != report_const.OK:
            raise RuntimeError("I2C write error.")

    def _i2c_writeto_direct(self, addr, buf, stop=True):
        report_id = report_const.I2C0_WRITE if self.i2c_index == 0 else report_const.I2C1_WRITE
        stop_flag = 0x01 if stop else 0x00
        start = 0
        end = len(buf)
        while (end - start) > 0:
            remain_bytes = end - start
            chunk = min(remain_bytes, report_const.HID_REPORT_SIZE - 7)
            res = self._device.send_report(
                bytes([report_id, addr, stop_flag]) + remain_bytes.to_bytes(4, byteorder='little') + buf[start : (start + chunk)])
            if res[1] != report_const.OK:
                raise RuntimeError("I2C write error.")

            start += chunk
