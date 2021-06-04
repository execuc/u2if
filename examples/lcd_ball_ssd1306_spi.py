# Example and ssd1306 driver derived from https://github.com/adafruit/Adafruit_CircuitPython_SSD1306, whhich is licensed :
# SPDX-FileCopyrightText: 2021 ladyada for Adafruit Industries
# SPDX-License-Identifier: MIT


from external.ssd1306 import SSD1306_SPI, SET_DISP_START_LINE
from machine import SPI, Pin, u2if


# Connect an I2C SSD1306 SPI (128x64)
# Pinout:
# GND: GND
# VCC: +3.3v
# D0: GP18 (SPI0_CLK)
# D1: GP19 (SPI0_MOSI)
# RES: GP8
# DC: GP9
# CS: GP17 (SPI0_CS1)

spi = SPI(spi_index=0)
spi.init(baudrate=10000000)

reset = Pin(u2if.GP8, Pin.OUT, value=Pin.LOW)
dc = Pin(u2if.GP9, Pin.OUT, value=Pin.LOW)
cs = Pin(u2if.GP17, Pin.OUT, value=Pin.HIGH)

oled = SSD1306_SPI(128, 64, spi, dc, reset, cs)

# Helper function to draw a circle from a given position with a given radius
# This is an implementation of the midpoint circle algorithm,
# see https://en.wikipedia.org/wiki/Midpoint_circle_algorithm#C_example for details
def draw_circle(xpos0, ypos0, rad, col=1):
    x = rad - 1
    y = 0
    dx = 1
    dy = 1
    err = dx - (rad << 1)
    while x >= y:
        oled.pixel(xpos0 + x, ypos0 + y, col)
        oled.pixel(xpos0 + y, ypos0 + x, col)
        oled.pixel(xpos0 - y, ypos0 + x, col)
        oled.pixel(xpos0 - x, ypos0 + y, col)
        oled.pixel(xpos0 - x, ypos0 - y, col)
        oled.pixel(xpos0 - y, ypos0 - x, col)
        oled.pixel(xpos0 + y, ypos0 - x, col)
        oled.pixel(xpos0 + x, ypos0 - y, col)
        if err <= 0:
            y += 1
            err += dy
            dy += 2
        if err > 0:
            x -= 1
            dx += 2
            err += dx - (rad << 1)


# initial center of the circle
center_x = 63
center_y = 15
# how fast does it move in each direction
x_inc = 1
y_inc = 1
# what is the starting radius of the circle
radius = 8

# start with a blank screen
oled.fill(0)
# we just blanked the framebuffer. to push the framebuffer onto the display, we call show()
oled.show()
while True:
    # undraw the previous circle
    draw_circle(center_x, center_y, radius, col=0)

    # if bouncing off right
    if center_x + radius >= oled.width:
        # start moving to the left
        x_inc = -1
    # if bouncing off left
    elif center_x - radius < 0:
        # start moving to the right
        x_inc = 1

    # if bouncing off top
    if center_y + radius >= oled.height:
        # start moving down
        y_inc = -1
    # if bouncing off bottom
    elif center_y - radius < 0:
        # start moving up
        y_inc = 1

    # go more in the current direction
    center_x += x_inc
    center_y += y_inc

    # draw the new circle
    draw_circle(center_x, center_y, radius)
    # show all the changes we just made
    oled.show()
