#include "console.h"
#include "mcu/mcu.h"
#include <stdio.h>
#include "mcu/uart.h"
#include <stdlib.h>
#include "console_cmds.h"
#include "utils/cmd.h"

#define CONSOLE_BUFFER 64
extern concmd_data_t procs[];

static char buffer[CONSOLE_BUFFER];
static uint8_t  echo    = 1;
static uint8_t  len     = 0;

void Console_init() {}

void Console_update() {
    uint8_t c = uart0_getc();
    if (c) {
        if (len == 0 && c == '$') { echo = 0; }

        if (echo) {
            uart0_putc(c);
            if (c == '\r') uart0_putc('\n');
        }

        if (c == 0x08 || c == 0x7F) {
            if (len) buffer[--len] = 0;
        } else {
            if (c != '\n') buffer[len++] = c;
        }

        if (len == sizeof(buffer) || c == '\r') {
            buffer[--len] = 0;
            len = 0;
            echo = 1;
            for(concmd_data_t* f = procs; f->cmd; f++) {
                if (!compareCmds(buffer, f->cmd)) {
                    f->func(getFirstArg(buffer));
                    return;
                }
            }
            if (*buffer) { printf_P(PSTR("?command %s\n"), buffer); }
        }
    }

}

void Console_update_100ms() {

}

void Console_update_1S() {
    if (print_min_max) {
        printf_P(PSTR("ADC ???\n"));
//         printf_P(PSTR("ADC 3 min %0000u max %0000u\n"), AdcMega_get_AD_min(3), AdcMega_get_AD_max(3));
//         printf_P(PSTR("ADC 4 min %0000u max %0000u\n"), AdcMega_get_AD_min(4), AdcMega_get_AD_max(4));
//         printf_P(PSTR("ADC 5 min %0000u max %0000u\n"), AdcMega_get_AD_min(5), AdcMega_get_AD_max(5));
//         printf_P(PSTR("ADC 6 min %0000u max %0000u\n"), AdcMega_get_AD_min(6), AdcMega_get_AD_max(6));
//         printf_P(PSTR("ADC 7 min %0000u max %0000u\n"), AdcMega_get_AD_min(7), AdcMega_get_AD_max(7));
    }
}




