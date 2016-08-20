#!/bin/sh

docker run -it -v /dev/ttyUSB0:/dev/ttyUSB0 grepherd/esp
cd test
esptool.py write_flash 0 blinky-0x00000.bin 0x40000 blinky-0x40000.bin
cd ..