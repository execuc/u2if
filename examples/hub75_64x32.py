import time
from machine import HUB75
from PIL import Image, ImageSequence

leds = HUB75(64, 32)
im = Image.open('./external/ressources/sprite/nyan_64x32.gif')

frames = []
for frame in ImageSequence.Iterator(im):
    array = []
    frame_rgb = frame.convert('RGB')
    for y in range(frame.height):
        for x in range(frame.width):
            array.append(frame_rgb.getpixel((x,y)))
    frames.append({'duration': frame.info['duration'], 'rgb': array})

while True:
    for frame in frames:
        leds.write(frame['rgb'])
        time.sleep(frame['duration']/1000.)
