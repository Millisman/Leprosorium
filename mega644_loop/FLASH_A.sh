#!/bin/bash

avrdude -v -p m644p -carduino -P /dev/ttyUSB0 -b 115200 -U flash:w:evse644p_ngev.hex:i
