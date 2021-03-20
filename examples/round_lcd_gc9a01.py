#!/usr/bin/python
# -*- coding: UTF-8 -*-

# Round lcd GC9A01 (240x240) example, from Waveshare team. Modified to work on micropython

#import chardet
import os
import sys
import time
import logging
from external.gc9a01 import Lcd
from PIL import Image, ImageDraw,ImageFont
from machine import SPI, Pin, u2if

try:
    # display with hardware SPI:
    ''' Warning!!!Don't  creation of multiple displayer objects!!! '''
    #disp = LCD_1inch28.LCD_1inch28(spi=SPI.SpiDev(bus, device),spi_freq=10000000,rst=RST,dc=DC,bl=BL)
    spi = SPI(spi_index=0)
    spi.init(baudrate=1000000)

    rst = Pin(u2if.GP_6)
    dc = Pin(u2if.GP_7)
    cs = Pin(u2if.GP_8)
    bl = Pin(u2if.GP_9)

    disp = Lcd(spi, dc, rst, cs, bl, bl_freq=1000)
    # Initialize library.
    disp.Init()
    disp.bl_DutyCycle(65000)
    # Clear display.
    disp.clear()

    # Create blank image for drawing.
    image1 = Image.new("RGB", (disp.width, disp.height), "BLACK")
    draw = ImageDraw.Draw(image1)

    #logging.info("draw point")
    #draw.rectangle((Xstart,Ystart,Xend,Yend), fill = "color")
    print("draw circle")
    draw.arc((1,1,239,239),0, 360, fill =(0,0,255))
    draw.arc((2,2,238,238),0, 360, fill =(0,0,255))
    draw.arc((3,3,237,237),0, 360, fill =(0,0,255))

    print("draw dial line")
    draw.line([(120, 1),(120, 12)], fill = (128,255,128),width = 4)
    draw.line([(120, 227),(120, 239)], fill = (128,255,128),width = 4)
    draw.line([(1,120),(12,120)], fill = (128,255,128),width = 4)
    draw.line([(227,120),(239,120)], fill = (128,255,128),width = 4)

    # print("draw text")
    Font1 = ImageFont.truetype("./external/ressources/Font/Font01.ttf",25)
    Font2 = ImageFont.truetype("./external/ressources/Font/Font01.ttf",35)
    Font3 = ImageFont.truetype("./external/ressources/Font/Font02.ttf",32)
    #
    draw.text((40, 50), 'WaveShare', fill = (128,255,128),font=Font2)
    text= u"微雪电子"
    draw.text((74, 150),text, fill = "WHITE",font=Font3)

    print("draw pointer line")
    draw.line([(120, 120),(70, 70)], fill = "YELLOW",width = 3)
    draw.line([(120, 120),(176, 64)], fill = "BLUE",width = 3)
    draw.line([(120, 120),(120 ,210)], fill = "RED",width = 3)

    # im_r=image1.rotate(180)
    # disp.ShowImage(im_r)
    disp.ShowImage(image1)
    time.sleep(3)
    print("show image")
    image = Image.open('./external/ressources/pic/LCD_1inch28_1.jpg')
    #im_r=image.rotate(180)
    #disp.ShowImage(im_r)
    disp.ShowImage(image)
    time.sleep(3)
    #disp.module_exit()
    #logging.info("quit:")
    print("quit")
except IOError as e:
    #logging.info(e)
    print(e)
except KeyboardInterrupt:
    disp.module_exit()
    #logging.info("quit:")
    print("quit:")
    exit()
