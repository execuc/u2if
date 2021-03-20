import serial.tools.list_ports


class Singleton(type):
    _instances = {}

    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            cls._instances[cls] = super(Singleton, cls).__call__(*args, **kwargs)
        #else:
        #    cls._instances[cls].__init__(*args, **kwargs)
        return cls._instances[cls]


def find_serial_port(vid, pid, serial_number_str=None):
    ports = serial.tools.list_ports.comports()

    for check_port in ports:
        if hasattr(serial.tools, 'list_ports_common'):
            if check_port.vid == vid and check_port.pid == pid and \
                    (serial_number_str is None or check_port.serial_number == serial_number_str):
                return check_port.device
    return None


def get_event_bytes_from_gpio_event(gpio, event_id):
    return bytes(((event_id & 0b11) >> 6) | (gpio & 0b111111))
