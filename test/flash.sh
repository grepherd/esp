#!/bin/bash

esptool.py --port /dev/ttyUSB0 write_flash -fm qio -fs 4m 0x00000 blinky-0x00000.bin 0x40000 
blinky-0x40000.bin
