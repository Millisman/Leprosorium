#include "console_cmds.h"
#include "mcu/mcu.h"
#include "mcu/uart.h"
#include <stdio.h>
#include <stdlib.h>
#include "console.h"

concmd_data_t procs[] = {
    {pstr_minmax,   pstr_minmax_h,  cmd_minmax},
    {pstr_reboot,   pstr_reboot_h,  cmd_reboot},
    {pstr_cd,       pstr_cd_h,      cmd_cd},
    {pstr_uptime,   pstr_uptime_h,  cmd_uptime},
    {pstr_help,     pstr_help_h,    cmd_help},
    {0, 0, 0}
};


const char pstr_cd[]        PROGMEM = "cd";
const char pstr_cd_h[]      PROGMEM = "help string for cd";

void cmd_cd(char* args) {
    (void) args;
    printf_P(PSTR("CD\n"));
}

const char pstr_uptime[]    PROGMEM = "uptime";
const char pstr_uptime_h[]  PROGMEM = "help string for uptime";

void cmd_uptime(char* args) {
    (void) args;
    printf_P(PSTR("%lu sec\n"), uptime_sec());
}

const char pstr_reboot[]    PROGMEM = "reboot";
const char pstr_reboot_h[]  PROGMEM = "help string for reboot";

void cmd_reboot(char* args) {
    (void) args;
    //     uart0_flush();
    //     wdt_disable();
    cli();
    //     _delay_ms(200);
    //     redraw
    wdt_enable(WDTO_15MS);
    while (1) { }
    //     typedef void (*do_reboot_t)(void);
    //     const do_reboot_t do_reboot = (do_reboot_t)((FLASHEND - 511) >> 1);
    //     TCCR0A = TCCR1A = TCCR2A = 0; // make sure interrupts are off and timers are reset.
    //     MCUSR = 0;
    //     do_reboot();
}


uint8_t print_min_max;
const char pstr_minmax[]    PROGMEM = "minmax";
const char pstr_minmax_h[]  PROGMEM = "help string for minmax 1/0";
void cmd_minmax(char *args) {
    if (args) {
        print_min_max = atoi(args);
    }
}



const char pstr_help[]      PROGMEM = "help";
const char pstr_help_h[]    PROGMEM = "help string for help";
void cmd_help (char *args) {
    (void) args;
    for (concmd_data_t* f = procs; f->cmd; f++) {
        uart0_puts_p(f->cmd);
        uart0_puts_p(PSTR("\t\t"));
        uart0_puts_p(f->help);
        uart0_puts_p(PSTR("\r\n"));
    }
}
