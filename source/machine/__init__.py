from .i2c import I2C
from .pin import Pin
from .group_pin import GroupPin
from .signal import Signal
from .pwm import PWM
from .adc import ADC
from .uart import UART
from .spi import SPI
from .i2s import I2S
from .ws2812b import WS2812B
from .hub75 import HUB75
from .u2if_const import u2if
from .u2if import Device


def select_interface(sn):
    Device(serial_number_str=sn)


def unique_id():
    device = Device()
    return device.get_serial_number()


def firmware_version():
    device = Device()
    return device.get_firmware_version()

