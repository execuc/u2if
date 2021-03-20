# Round lcd driver GC9A01 (240x240). Driver derived from Waveshare team which which is originally licensed : see below
# Minor changes to work on micropython
# ********************************************************************************************************

# /*****************************************************************************
# * | File        :	  epdconfig.py
# * | Author      :   Waveshare team
# * | Function    :   Hardware underlying interface
# * | Info        :
# *----------------
# * | This version:   V1.0
# * | Date        :   2019-06-21
# * | Info        :
# ******************************************************************************
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

import time
import numpy as np
from machine import Pin, PWM

class BaseLcd:
    def __init__(self, spi, dc, rst, cs, bl, bl_freq=1000):
        #def __init__(self,spi=spidev.SpiDev(0,0),spi_freq=40000000,rst = 27,dc = 25,bl = 18,bl_freq=1000,i2c=None,i2c_freq=100000):
        #    import RPi.GPIO
        self.np = np
        self.SPI = spi
        self.RST_PIN = rst
        self.DC_PIN = dc
        self.BL_PIN = bl
        self.CS_PIN = cs
        #self.SPEED  =spi_freq
        self.BL_freq=bl_freq
        #self.GPIO = RPi.GPIO
        #self.GPIO.cleanup()
        # self.GPIO.setmode(self.GPIO.BCM)
        # self.GPIO.setwarnings(False)
        # self.GPIO.setup(self.RST_PIN,   self.GPIO.OUT)
        # self.GPIO.setup(self.DC_PIN,    self.GPIO.OUT)
        # self.GPIO.setup(self.BL_PIN,    self.GPIO.OUT)
        # self.GPIO.output(self.BL_PIN,   self.GPIO.HIGH)
        self.RST_PIN.init(Pin.OUT)
        self.DC_PIN.init(Pin.OUT)
        self.BL_PIN.init(Pin.OUT, Pin.HIGH)
        self.CS_PIN.init(Pin.OUT, Pin.HIGH)
        #Initialize SPI
        self.SPI = spi
        # if self.SPI!=None :
        #     self.SPI.max_speed_hz = spi_freq
        #     self.SPI.mode = 0b00
        self._pwm = PWM(self.BL_PIN)
        self._pwm.freq(self.BL_freq)
        self._pwm.duty_u16(32000)

    def digital_write(self, pin, value):
        #self.GPIO.output(pin, value)
        pin.value(value)

    def digital_read(self, pin):
        #return self.GPIO.input(pin)
        return pin.value()

    def delay_ms(self, delaytime):
        time.sleep(delaytime / 1000.0)

    def spi_writebyte(self, data):
        if self.SPI!=None :
            #self.SPI.writebytes(data)
            self.SPI.write(data)

    def bl_DutyCycle(self, duty):
        #self._pwm.ChangeDutyCycle(duty)
        self._pwm.duty_u16(duty)

    def bl_Frequency(self,freq):
        #self._pwm.ChangeFrequency(freq)
        self._pwm.freq(freq)

    def module_init(self):
        # self.RST_PIN.setup(self.DC_PIN, self.GPIO.OUT)
        # self.GPIO.setup(self.DC_PIN, self.GPIO.OUT)
        # self.GPIO.setup(self.BL_PIN, self.GPIO.OUT)
        self.RST_PIN.init(Pin.OUT)
        self.DC_PIN.init(Pin.OUT)
        #self.BL_PIN.init(Pin.OUT)
        # self._pwm=self.GPIO.PWM(self.BL_PIN,self.BL_freq)
        # self._pwm.start(100)
        # if self.SPI!=None :
        #     self.SPI.max_speed_hz = self.SPEED
        #     self.SPI.mode = 0b00
        return 0


class Lcd(BaseLcd):

    width = 240
    height = 240

    def command(self, cmd):
        #self.digital_write(self.DC_PIN, self.GPIO.LOW)
        self.CS_PIN.value(Pin.LOW)
        self.DC_PIN.value(Pin.LOW)
        self.spi_writebyte([cmd])
        self.CS_PIN.value(Pin.HIGH)

    def data(self, val):
        #self.digital_write(self.DC_PIN, self.GPIO.HIGH)
        self.CS_PIN.value(Pin.LOW)
        self.DC_PIN.value(Pin.HIGH)
        self.spi_writebyte([val])
        self.CS_PIN.value(Pin.HIGH)

    def reset(self):
        """Reset the display"""
        #self.GPIO.output(self.RST_PIN,self.GPIO.HIGH)
        self.RST_PIN.value(Pin.HIGH)
        time.sleep(0.01)
        #self.GPIO.output(self.RST_PIN,self.GPIO.LOW)
        self.RST_PIN.value(Pin.LOW)
        time.sleep(0.01)
        #self.GPIO.output(self.RST_PIN,self.GPIO.HIGH)
        self.RST_PIN.value(Pin.HIGH)
        time.sleep(0.01)

    def Init(self):
        """Initialize dispaly"""
        self.module_init()
        self.reset()

        self.command(0xEF)
        self.command(0xEB)
        self.data(0x14)

        self.command(0xFE)
        self.command(0xEF)

        self.command(0xEB)
        self.data(0x14)

        self.command(0x84)
        self.data(0x40)

        self.command(0x85)
        self.data(0xFF)

        self.command(0x86)
        self.data(0xFF)

        self.command(0x87)
        self.data(0xFF)

        self.command(0x88)
        self.data(0x0A)

        self.command(0x89)
        self.data(0x21)

        self.command(0x8A)
        self.data(0x00)

        self.command(0x8B)
        self.data(0x80)

        self.command(0x8C)
        self.data(0x01)

        self.command(0x8D)
        self.data(0x01)

        self.command(0x8E)
        self.data(0xFF)

        self.command(0x8F)
        self.data(0xFF)


        self.command(0xB6)
        self.data(0x00)
        self.data(0x20)

        self.command(0x36)
        self.data(0x08)

        self.command(0x3A)
        self.data(0x05)


        self.command(0x90)
        self.data(0x08)
        self.data(0x08)
        self.data(0x08)
        self.data(0x08)

        self.command(0xBD)
        self.data(0x06)

        self.command(0xBC)
        self.data(0x00)

        self.command(0xFF)
        self.data(0x60)
        self.data(0x01)
        self.data(0x04)

        self.command(0xC3)
        self.data(0x13)
        self.command(0xC4)
        self.data(0x13)

        self.command(0xC9)
        self.data(0x22)

        self.command(0xBE)
        self.data(0x11)

        self.command(0xE1)
        self.data(0x10)
        self.data(0x0E)

        self.command(0xDF)
        self.data(0x21)
        self.data(0x0c)
        self.data(0x02)

        self.command(0xF0)
        self.data(0x45)
        self.data(0x09)
        self.data(0x08)
        self.data(0x08)
        self.data(0x26)
        self.data(0x2A)

        self.command(0xF1)
        self.data(0x43)
        self.data(0x70)
        self.data(0x72)
        self.data(0x36)
        self.data(0x37)
        self.data(0x6F)


        self.command(0xF2)
        self.data(0x45)
        self.data(0x09)
        self.data(0x08)
        self.data(0x08)
        self.data(0x26)
        self.data(0x2A)

        self.command(0xF3)
        self.data(0x43)
        self.data(0x70)
        self.data(0x72)
        self.data(0x36)
        self.data(0x37)
        self.data(0x6F)

        self.command(0xED)
        self.data(0x1B)
        self.data(0x0B)

        self.command(0xAE)
        self.data(0x77)

        self.command(0xCD)
        self.data(0x63)


        self.command(0x70)
        self.data(0x07)
        self.data(0x07)
        self.data(0x04)
        self.data(0x0E)
        self.data(0x0F)
        self.data(0x09)
        self.data(0x07)
        self.data(0x08)
        self.data(0x03)

        self.command(0xE8)
        self.data(0x34)

        self.command(0x62)
        self.data(0x18)
        self.data(0x0D)
        self.data(0x71)
        self.data(0xED)
        self.data(0x70)
        self.data(0x70)
        self.data(0x18)
        self.data(0x0F)
        self.data(0x71)
        self.data(0xEF)
        self.data(0x70)
        self.data(0x70)

        self.command(0x63)
        self.data(0x18)
        self.data(0x11)
        self.data(0x71)
        self.data(0xF1)
        self.data(0x70)
        self.data(0x70)
        self.data(0x18)
        self.data(0x13)
        self.data(0x71)
        self.data(0xF3)
        self.data(0x70)
        self.data(0x70)

        self.command(0x64)
        self.data(0x28)
        self.data(0x29)
        self.data(0xF1)
        self.data(0x01)
        self.data(0xF1)
        self.data(0x00)
        self.data(0x07)

        self.command(0x66)
        self.data(0x3C)
        self.data(0x00)
        self.data(0xCD)
        self.data(0x67)
        self.data(0x45)
        self.data(0x45)
        self.data(0x10)
        self.data(0x00)
        self.data(0x00)
        self.data(0x00)

        self.command(0x67)
        self.data(0x00)
        self.data(0x3C)
        self.data(0x00)
        self.data(0x00)
        self.data(0x00)
        self.data(0x01)
        self.data(0x54)
        self.data(0x10)
        self.data(0x32)
        self.data(0x98)

        self.command(0x74)
        self.data(0x10)
        self.data(0x85)
        self.data(0x80)
        self.data(0x00)
        self.data(0x00)
        self.data(0x4E)
        self.data(0x00)

        self.command(0x98)
        self.data(0x3e)
        self.data(0x07)

        self.command(0x35)
        self.command(0x21)

        self.command(0x11)
        time.sleep(0.12)
        self.command(0x29)
        time.sleep(0.02)

    def SetWindows(self, Xstart, Ystart, Xend, Yend):
        #set the X coordinates
        self.command(0x2A)
        self.data(0x00)               #Set the horizontal starting point to the high octet
        self.data(Xstart)      #Set the horizontal starting point to the low octet
        self.data(0x00)               #Set the horizontal end to the high octet
        self.data(Xend - 1) #Set the horizontal end to the low octet

        #set the Y coordinates
        self.command(0x2B)
        self.data(0x00)
        self.data(Ystart)
        self.data(0x00)
        self.data(Yend - 1)

        self.command(0x2C)

    def ShowImage(self,Image):
        """Set buffer to value of Python Imaging Library image."""
        """Write display buffer to physical display"""
        imwidth, imheight = Image.size
        if imwidth != self.width or imheight != self.height:
            raise ValueError('Image must be same dimensions as display \
                ({0}x{1}).' .format(self.width, self.height))
        img = self.np.asarray(Image)
        pix = self.np.zeros((self.width,self.height,2), dtype = self.np.uint8)
        pix[...,[0]] = self.np.add(self.np.bitwise_and(img[...,[0]],0xF8),self.np.right_shift(img[...,[1]],5))
        pix[...,[1]] = self.np.add(self.np.bitwise_and(self.np.left_shift(img[...,[1]],3),0xE0),self.np.right_shift(img[...,[2]],3))
        pix = pix.flatten().tolist()
        self.SetWindows ( 0, 0, self.width, self.height)
        #self.digital_write(self.DC_PIN,self.GPIO.HIGH)
        self.CS_PIN.value(Pin.LOW)
        self.DC_PIN.value(Pin.HIGH)
        #for i in range(0,len(pix),4096):
        #    self.spi_writebyte(pix[i:i+4096])
        self.spi_writebyte(pix)
        self.CS_PIN.value(Pin.HIGH)

    def clear(self):
        """Clear contents of image buffer"""
        _buffer = [0xff]*(self.width * self.height * 2)
        self.SetWindows ( 0, 0, self.width, self.height)
        #self.digital_write(self.DC_PIN,self.GPIO.HIGH)
        self.CS_PIN.value(Pin.LOW)
        self.DC_PIN.value(Pin.HIGH)
        #for i in range(0,len(_buffer),4096):
        #    self.spi_writebyte(_buffer[i:i+4096])
        self.spi_writebyte(_buffer)
        self.CS_PIN.value(Pin.HIGH)

