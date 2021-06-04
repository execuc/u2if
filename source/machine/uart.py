import queue
import time
from .u2if import Device
from . import u2if_const as report_const


class UART:
    def __init__(self, uart_index=0):
        self._initialized = False
        if uart_index >= 2:
            raise RuntimeError("Uart index error.")
        self.uart_index = uart_index
        self._rx_buffer = queue.Queue()
        self.end_line_char = 10
        self._device = Device()

    def __del__(self):
        self.deinit()

    def init(self, baudrate=9600):
        report_id = report_const.UART0_INIT if self.uart_index == 0 else report_const.UART1_INIT
        res = self._device.send_report(bytes([report_id, 0x00])
                                       + baudrate.to_bytes(4, byteorder='little'))
        if res[1] != report_const.OK:
            raise RuntimeError("Uart init error.")
        self._initialized = True
        return True

    def deinit(self):
        if not self._initialized:
            return
        report_id = report_const.UART0_DEINIT if self.uart_index == 0 else report_const.UART1_DEINIT
        res = self._device.send_report(bytes([report_id]))
        if res[1] != report_const.OK:
            raise RuntimeError("Uart deinit error.")

    def any(self):
        self._read_rx_buffer()
        return self._rx_buffer.qsize()

    def readinto(self, buffer, nbBytes=-1, timeout=None):
        max_char = len(buffer)
        if nbBytes > 0:
            max_char = min(len(buffer), nbBytes)
        tmp = self.read(max_char, timeout)
        for i in range(len(tmp)):
            buffer[i] = tmp[i]

    def read(self, nbBytes=-1, timeout=None):
        start_time = time.time()
        while True:
            self._read_rx_buffer()
            if nbBytes == -1 or self._rx_buffer.qsize() >= nbBytes:
                break
            elapsed_time_s = time.time() - start_time
            if timeout is not None and elapsed_time_s > timeout:
                break

        res_array = []
        nb_char = min(nbBytes, self._rx_buffer.qsize()) if nbBytes > 0 else self._rx_buffer.qsize()
        for i in range(nb_char):
            res_array.append(self._rx_buffer.get())
        return res_array

    def readline(self, timeout=None):
        start_time = time.time()
        res_array = []
        while True:
            self._read_rx_buffer()
            # test \n
            if self.end_line_char in list(self._rx_buffer.queue):
                break
            elapsed_time_s = time.time() - start_time
            if timeout is not None and elapsed_time_s > timeout:
                break

        if self.end_line_char in list(self._rx_buffer.queue):
            while True:
                res_array.append(self._rx_buffer.get())
                if res_array[-1] == self.end_line_char:
                    break

        return res_array

    def _read_rx_buffer(self):
        report_id = report_const.UART0_READ if self.uart_index == 0 else report_const.UART1_READ
        res = self._device.send_report(bytes([report_id]))
        if res[1] != report_const.OK:
            raise RuntimeError("Uart read rx buffer error.")
        payload_size = res[2]
        for i in range(payload_size):
            self._rx_buffer.put(res[3+i])

    def write(self, buffer):
        report_id = report_const.UART0_WRITE if self.uart_index == 0 else report_const.UART1_WRITE
        start = 0
        end = len(buffer)
        while (end - start) > 0:
            remain_bytes = end - start
            chunk = min(remain_bytes, report_const.HID_REPORT_SIZE - 3)
            res = self._device.send_report(bytes([report_id, chunk]) + buffer[start: (start + chunk)])
            if res[1] != report_const.OK:
                raise RuntimeError("Uart write error.")
            start += chunk
