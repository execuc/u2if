from machine import WS2812B, u2if

nb_led = 100

leds = WS2812B(u2if.GP21)
array = []
for i in range(nb_led):
    array.append(((i+1) & 0xFF, (i+2) & 0xFF, (i+3) & 0xFF))

leds.write(array)
