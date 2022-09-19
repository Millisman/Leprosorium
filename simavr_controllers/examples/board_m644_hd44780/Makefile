target= controller_m644p
simavr = ../../

IPATH = .
IPATH += ../parts
IPATH += ${simavr}/include
IPATH += ${simavr}/simavr/sim

VPATH = .
VPATH += ../parts

LDFLAGS += -lpthread

include ../Makefile.opengl

all: obj ${target}

include ${simavr}/Makefile.common

board = ${OBJ}/../${target}.elf


${board} : ${OBJ}/uart_modem.o
${board} : ${OBJ}/hd44780.o
${board} : ${OBJ}/hd44780_glut.o
${board} : ${OBJ}/${target}.o




${target}: ${board}
	@echo $@ done

clean: clean-${OBJ}
	rm -rf *.a ${target} *.vcd .*.swo .*.swp .*.swm .*.swn
