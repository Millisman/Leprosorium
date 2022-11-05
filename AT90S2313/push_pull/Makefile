COMPILE = avr-gcc -Wall -Os -I. -mmcu=atmega328p

OBJECTS = main.o

# symbolic targets:
all:	main.bin

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	avrdude -c usbtiny -p 2313  -U flash:w:main.hex

# Fuse low byte:
# 0xef = 1 1 1 0   1 1 1 1
#        ^ ^ \+/   \--+--/
#        | |  |       +------- CKSEL 3..0 (clock selection -> crystal @ 12 MHz)
#        | |  +--------------- SUT 1..0 (BOD enabled, fast rising power)
#        | +------------------ CKOUT (clock output on CKOUT pin -> disabled)
#        +-------------------- CKDIV8 (divide clock by 8 -> don't divide)
#
# Fuse high byte:
# 0xdb = 1 1 0 1   1 0 1 1
#        ^ ^ ^ ^   \-+-/ ^
#        | | | |     |   +---- RSTDISBL (disable external reset -> enabled)
#        | | | |     +-------- BODLEVEL 2..0 (brownout trigger level -> 2.7V)
#        | | | +-------------- WDTON (watchdog timer always on -> disable)
#        | | +---------------- SPIEN (enable serial programming -> enabled)
#        | +------------------ EESAVE (preserve EEPROM on Chip Erase -> not preserved)
#        +-------------------- DWEN (debug wire enable)
fuse_tiny2313:	# only needed for attiny2313
	$(UISP) --wr_fuse_l=0xef --wr_fuse_h=0xdb


clean:
	rm -f main.bin main.lst main.obj main.cof main.list main.map main main.hex *.o

# file targets:
main:	$(OBJECTS)
	$(COMPILE) -o main $(OBJECTS)

main.bin:	main
	rm -f main.bin
	avr-objcopy -j .text -j .data -O binary main main.bin
	avr-objcopy -j .text -j .data -O ihex   main main.hex
	#./checksize main
# do the checksize script as our last action to allow successful compilation
# on Windows with WinAVR where the Unix commands will fail.

disasm:	main
	avr-objdump -d main

cpp:
	$(COMPILE) -E main.c

flash_SPI:
	avrdude -c usbtiny -p atmega328p -U flash:w:main.bin
