import time
from machine import u2if, Pin, PWM

# Connect a servo on GP_6

MIN_DUTY_MS = 1000
MAX_DUTY_MS = 2000

servo_pwm = PWM(Pin(u2if.GP_6))
servo_pwm.freq(50)

duty = MIN_DUTY_MS
direction = 1

while True:
    duty += direction
    if duty > MAX_DUTY_MS:
        duty = MAX_DUTY_MS
        direction = -direction
    elif duty < MIN_DUTY_MS:
        duty = MIN_DUTY_MS
        direction = -direction
    servo_pwm.duty_ns(duty*1000)
    time.sleep(0.005)

