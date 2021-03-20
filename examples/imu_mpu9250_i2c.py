# From https://github.com/tuupola/micropython-mpu9250
# Minor changes to work with u2if module

import time
from machine import I2C, Pin
from external.imu.mpu9250 import MPU9250

i2c = I2C(i2c_index=0, frequency=400000) # , pullup=True
sensor = MPU9250(i2c)

print("MPU9250 id: " + hex(sensor.whoami))

while True:
    print(sensor.acceleration)
    print(sensor.gyro)
    print(sensor.magnetic)
    print(sensor.temperature)

    #time.sleep(1)
    time.sleep(0.1)
