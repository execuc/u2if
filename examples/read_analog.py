import time
from machine import u2if, ADC

analog = ADC(u2if.GP26)

while True:
    print('Analog GP26_ADC0: %d' % analog.value())
    time.sleep(1)
