import time
from machine import u2if, Pin, PWM

led_pwm = PWM(Pin(u2if.GP_3))
led_pwm.freq(1000)

while True:
    for i in range(0, 65535, 50):
        led_pwm.duty_u16(i)
        time.sleep(0.005)

    time.sleep(1)

    for i in reversed(range(0, 65535, 50)):
        led_pwm.duty_u16(i)
        time.sleep(0.005)
