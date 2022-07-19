EXEC=GCC_Linux
SRCS:=$(wildcard *.c) $(wildcard nOS/src/*.c) $(wildcard nOS/src/port/GCC/AVR/*.c)
INCS=-Iconfig -InOS/inc -InOS/inc/port/GCC/AVR -I/usr/avr/sys-root/include -I.
LIBS=
#######################################################################################

CC=avr-gcc
OC=avr-objcopy
CFLAGS  = -c -g -Os -w -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -flto -Wall -std=c99 -D_GNU_SOURCE -mmcu=atmega644p -DF_CPU=20000000L
LDFLAGS =

OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(EXEC)

.c.o:
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

$(EXEC): $(OBJS)

	$(CC) -mmcu=atmega644p -o main.elf $(OBJS) -lm -emain
	$(OC) -O ihex -R .eeprom  main.elf main.hex
	# $(CC) $(OBJS) -o $@ $(LDFLAGS) $(LIBS)

clean:
	rm -f $(OBJS) $(EXEC)

