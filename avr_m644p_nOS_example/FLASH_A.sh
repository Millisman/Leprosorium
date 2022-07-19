avrdude -F -v -p m644p -carduino -P /dev/ttyUSB* -b 115200 -D -U flash:w:./main.hex:i
