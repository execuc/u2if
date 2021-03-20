# Example and ssd1306 driver derived from https://github.com/adafruit/Adafruit_CircuitPython_SSD1306, which is licensed :
# SPDX-FileCopyrightText: Melissa LeBlanc-Williams for Adafruit Industries
# SPDX-License-Identifier: MIT

import time
from PIL import Image, ImageDraw, ImageFont
from external.ssd1306 import SSD1306_I2C, SET_DISP_START_LINE
from machine import I2C

# Connect an I2C SSD1306 LCD (128x64)


def show_image(display, image, draw, font, font2):
    # write the current time to the display after each scroll
    draw.rectangle((0, 0, display.width, display.height * 2), outline=0, fill=0)
    text = time.strftime("%A")
    draw.text((0, 0), text, font=font, fill=255)
    text = time.strftime("%e %b %Y")
    draw.text((0, 14), text, font=font, fill=255)
    text = time.strftime("%X")
    draw.text((0, 36), text, font=font2, fill=255)
    display.image(image)
    display.show()


# Use second I2C port
i2c = I2C(i2c_index=1, frequency=400000) # , pullup=True

display = SSD1306_I2C(128, 64, i2c)
# Clear the display.  Always call show after changing pixels to make the display
# update visible!
display.fill(0)
display.show()
time.sleep(0.5)
display.fill(1)
display.show()
time.sleep(0.5)
display.fill(0)
display.show()

# # Set a pixel in the origin 0,0 position.
# display.pixel(0, 0, 1)
# # Set a pixel in the middle 64, 16 position.
# display.pixel(64, 32, 1)
# # Set a pixel in the opposite 127, 31 position.
# display.pixel(127, 63, 1)
# display.show()
image = Image.new("1", (display.width, display.height))
draw = ImageDraw.Draw(image)

# Load a font in 2 different sizes.
font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16)
font2 = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24)
offset = 0  # flips between 0 and 32 for double buffering


while True:
    for i in range(0, display.height // 2):
        offset = (offset + 1) % display.height
        display.write_cmd(SET_DISP_START_LINE | offset)
        show_image(display, image, draw, font, font2)

    for i in range(0, display.height // 2):
        show_image(display, image, draw, font, font2)
        time.sleep(0.05)

