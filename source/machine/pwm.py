from .u2if import Device
from . import u2if_const as report_const


class PWM:

    def __init__(self, pin, direction=None, pull=None, value=None):
        self._initialized = False
        self._device = Device()
        self.pin = pin
        self._initialized = self._init()

    def __del__(self):
        self.deinit()

    def _init(self):
        res = self._device.send_report(bytes([report_const.PWM_INIT_PIN, self.pin.id]))
        if res[1] != report_const.OK:
            raise RuntimeError("Pwm init error.")
        return True

    def deinit(self):
        if not self._initialized:
            return
        res = self._device.send_report(bytes([report_const.PWM_DEINIT_PIN, self.pin.id]))
        if res[1] != report_const.OK:
            raise RuntimeError("Pwm deinit error.")

    def freq(self, freq=None):
        if freq is None:
            return self._get_freq()
        else:
            return self._set_freq(freq)

    def duty_u16(self, duty=None):
        if duty is None:
            return self._get_duty_u16()
        else:
            return self._set_duty_u16(duty)

    def duty_ns(self, duty_ns=None):
        if duty_ns is None:
            return self._get_duty_ns()
        else:
            return self._set_duty_ns(duty_ns)

    # Private methods
    def _set_freq(self, freq):
        res = self._device.send_report(bytes([report_const.PWM_SET_FREQ, self.pin.id])
                                       + freq.to_bytes(4, byteorder='little'))
        if res[1] != report_const.OK and res[3] == 0x01:
            raise RuntimeError("Pwm different freq on same slice.")
        elif res[1] != report_const.OK and res[3] == 0x02:
            raise RuntimeError("Pwm freq too low.")
        elif res[1] != report_const.OK and res[3] == 0x03:
            raise RuntimeError("Pwm freq too high.")
        elif res[1] != report_const.OK:
            raise RuntimeError("Pwm freq error.")

    def _get_freq(self):
        res = self._device.send_report(bytes([report_const.PWM_GET_FREQ, self.pin.id]))
        if res[1] != report_const.OK:
            raise RuntimeError("Pwm get freq error.")
        return int.from_bytes(res[3:3+4], byteorder='little')

    def _get_duty_u16(self):
        res = self._device.send_report(bytes([report_const.PWM_GET_DUTY_U16, self.pin.id]))
        if res[1] != report_const.OK:
            raise RuntimeError("Pwm get duty_u16 error.")
        return int.from_bytes(res[3:3+2], byteorder='little')

    def _set_duty_u16(self, duty_u16):
        res = self._device.send_report(bytes([report_const.PWM_SET_DUTY_U16, self.pin.id])
                                       + duty_u16.to_bytes(2, byteorder='little'))
        if res[1] != report_const.OK:
            raise RuntimeError("Pwm set duty_u16 error.")

    def _get_duty_ns(self):
        res = self._device.send_report(bytes([report_const.PWM_GET_DUTY_NS, self.pin.id]))
        if res[1] != report_const.OK:
            raise RuntimeError("Pwm get duty_us error.")
        return int.from_bytes(res[3:3+4], byteorder='little')

    def _set_duty_ns(self, duty_ns):
        res = self._device.send_report(bytes([report_const.PWM_SET_DUTY_NS, self.pin.id])
                                       + duty_ns.to_bytes(4, byteorder='little'))
        if res[1] != report_const.OK and res[3] == 0x01:
            raise RuntimeError("Pwm too large duty_ns.")
        elif res[1] != report_const.OK:
            raise RuntimeError("Pwm duty_ns error.")
