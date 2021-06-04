# u2if PICO firmware

C++ firmware of the PICO to work with the u2if python module.
Binary file (u2if.uf2) is available in published release and to must be uploaded to the pico. It can be also be built.

## Upload method
Plug in the PICO while holding down the BOOTSEL button, then a USB flash storage should appear. Copy "u2if.uf2" on it.

## Working
The firmware makes the pico act like a USB device (generic HID and CDC). Each command is blocking and is done via the HID interface (64 byte report). For some operations, CDC is used to increase the transfer speed.

## Linux: UDEV rule
To make PICO with this firmware usable in non-root mode, add the following file (/etc/udev/rules.d/55-u2if.rules):

```bash
# PICO
SUBSYSTEM=="usb", ATTR{idVendor}=="cafe", ATTR{idProduct}=="4005", MODE="0666"
# Adafruit Feather
SUBSYSTEM=="usb", ATTR{idVendor}=="239a", ATTR{idProduct}=="00f1", MODE="0666"
# Adafruit ItsyBitsy
SUBSYSTEM=="usb", ATTR{idVendor}=="239a", ATTR{idProduct}=="00fd", MODE="0666"
# Adafruit QT2040 Trinkey
SUBSYSTEM=="usb", ATTR{idVendor}=="239a", ATTR{idProduct}=="0109", MODE="0666"
# Adafruit QTPY
SUBSYSTEM=="usb", ATTR{idVendor}=="239a", ATTR{idProduct}=="00f7", MODE="0666"

```

Then reboot or reload udev rules:

 * sudo udevadm control --reload-rules
 * sudo udevadm trigger

## Get pico-sdk submodule
In u2if directory:
 - git submodule update --init --recursive

## Build firmware
In u2if/firmware/source directory:
 - mkdir build
 - cd build
 - cmake ..
 - make

The firmware to upload to Pico is u2if/firmware/source/build/u2if.uf2

## Configure options

Compatible board can be:
 - PICO
 - FEATHER
 - ITSYBITSY 
 - QTPY
 - QT2040_TRINKEY

Default compilation is for PICO board. But target board can be selected during cmake call: cmake -DBOARD=qtpy ..

 Some interfaces can also be enabled(1)/disabled(0) during cmake:
  - ADC: -DADC_ENABLED=0 (default 1)
  - PWM: -DPWM_ENABLED=0 (default 1)
  - I2S: -DI2S_ALLOW=1   (default 0, work only for PICO board)
  - WS2812: -DWS2812_ENABLED=0 (default 1)

Note: for WS2812 interface, the maximum number of leds managed is 1000 but this can be modified by the parameter WS2812_SIZE. If we increase this number, the I2S interface must be deactivated because it uses a lot of ram.

Example for PICO board enabling I2S and setting 300 as maximum number of leds: ```cmake -DBOARD=PICO -DI2S_ALLOW=1 -DWS2812_SIZE=300 ..```

## Multiple target build
The build-all.sh script facilitates the generation of the different maps supported. It has to be launched form firmware directory and it will build uf2 firmware in firmware/release directory.
