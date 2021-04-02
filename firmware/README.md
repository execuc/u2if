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
SUBSYSTEM=="usb", ATTR{idVendor}=="cafe", ATTR{idProduct}=="4005", MODE="0666"
```

Then reboot or reload udev rules:

 * sudo udevadm control --reload-rules
 * sudo udevadm trigger

## Build firmware

In u2if/firmware/, clone pico-sdk (v1.1.0):
 - git clone https://github.com/raspberrypi/pico-sdk.git
 - cd pico-sdk
 - git reset --hard 1.1.0
 - git submodule update --init

In u2if/firmware/source directory:
 - mkdir build
 - cd build
 - cmake ..
 - make

The firmware to upload to Pico is u2if/firmware/source/build/u2if.uf2

