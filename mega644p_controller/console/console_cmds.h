#pragma once
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-attributes"
#include <stdint.h>


extern const char pstr_cd[];
extern const char pstr_cd_h[];
extern void cmd_cd(char* args);

extern const char pstr_uptime[];
extern const char pstr_uptime_h[];
extern void cmd_uptime(char* args);

extern const char pstr_reboot[];
extern const char pstr_reboot_h[];
extern void cmd_reboot(char* args);

extern const char pstr_help[];
extern const char pstr_help_h[];
extern void cmd_help(char* args);

extern uint8_t print_min_max;
extern const char pstr_minmax[];
extern const char pstr_minmax_h[];
extern void cmd_minmax(char *args);
