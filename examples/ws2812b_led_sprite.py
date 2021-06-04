from PIL import Image
import time
from machine import WS2812B, u2if

# For 16x16 led matrix
sprites = Image.open('./external/ressources/sprite/ninja_Yura_Zyuzyukin.png')

im1 = sprites.crop((0, 80, 16, 96))
im2 = sprites.crop((16, 80, 32, 96))
im3 = sprites.crop((32, 80, 48, 96))


def image_to_array(im, background_color=(0,0,0)):
    array = []
    for y in range(16):
        if y % 2 == 1:
            pixel_order = list(range(16))
        else:
            pixel_order = list(range(15, -1, -1))
        for x in pixel_order:
            pixel = im.getpixel((x,y))
            if pixel[3] == 0:
                array.append(background_color)
            else:
                array.append((pixel[0], pixel[1], pixel[2]))

    return array


leds = WS2812B(u2if.GP21)
background_color = (0, 0x20, 0)
for i in range(10):
    leds.write(image_to_array(im1, background_color))
    time.sleep(0.3)
    leds.write(image_to_array(im2, background_color))
    time.sleep(0.3)
    leds.write(image_to_array(im3, background_color))
    time.sleep(0.3)

leds.write(image_to_array(Image.open('./external/ressources/sprite/Magical_rainbow_star_Loel_cc0.png')))
time.sleep(5)

leds.write([(0,0,0) for i in range(256)])
