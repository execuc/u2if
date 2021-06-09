import time
from machine import u2if, Pin, GroupPin

# 64*32 led matrix with a bit banging hub75 protocol (so very slow). It uses GroupPin to select line
# through A,B,C and D.

# Initialize GPIO to output and set the value HIGH
led = Pin(u2if.GP20, Pin.OUT, value=Pin.HIGH)
time.sleep(1)
# Switch off the led
led.value(Pin.LOW)

R0 = Pin(u2if.GP0, Pin.OUT, value=Pin.LOW)
G0 = Pin(u2if.GP1, Pin.OUT, value=Pin.LOW)
B0 = Pin(u2if.GP2, Pin.OUT, value=Pin.LOW)
R1 = Pin(u2if.GP3, Pin.OUT, value=Pin.LOW)
G1 = Pin(u2if.GP4, Pin.OUT, value=Pin.LOW)
B1 = Pin(u2if.GP5, Pin.OUT, value=Pin.LOW)

A = Pin(u2if.GP6, Pin.OUT, value=Pin.LOW)
B = Pin(u2if.GP7, Pin.OUT, value=Pin.LOW)
C = Pin(u2if.GP8, Pin.OUT, value=Pin.LOW)
D = Pin(u2if.GP9, Pin.OUT, value=Pin.LOW)
lines = GroupPin([D, C, B, A])

CLK = Pin(u2if.GP11, Pin.OUT, value=Pin.HIGH)
STB = Pin(u2if.GP12, Pin.OUT, value=Pin.LOW)
OE = Pin(u2if.GP13, Pin.OUT, value=Pin.LOW)


for i in range(64):
    CLK.value(Pin.LOW)
    if i< 21:
        B0.value(Pin.HIGH)
        B1.value(Pin.HIGH)
        R0.value(Pin.LOW)
        R1.value(Pin.LOW)
        G0.value(Pin.LOW)
        G1.value(Pin.LOW)
    elif i < 43:
        B0.value(Pin.HIGH)
        B1.value(Pin.HIGH)
        R0.value(Pin.HIGH)
        R1.value(Pin.HIGH)
        G0.value(Pin.HIGH)
        G1.value(Pin.HIGH)
    else:
        R0.value(Pin.HIGH)
        R1.value(Pin.HIGH)
        B0.value(Pin.LOW)
        B1.value(Pin.LOW)
        G0.value(Pin.LOW)
        G1.value(Pin.LOW)
    CLK.value(Pin.HIGH)

OE.value(Pin.HIGH)
#A.value(Pin.LOW)
STB.value(Pin.HIGH)
STB.value(Pin.LOW)
OE.value(Pin.LOW)


while True:
    for i in range(16):
        #OE.value(Pin.HIGH)
        lines.value(i)
        #OE.value(Pin.LOW)

