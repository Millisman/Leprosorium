#include "cmd.h"
#include "mcu/mcu.h"
#include <avr/pgmspace.h>

uint8_t compareCmds(const char* s1, const char* s2) {
    while (*s1 && (*s1 != ' ') && (*s1 == pgm_read_byte(s2))) { s1++, s2++; }
    if (*s1 == ' ') { return pgm_read_byte(s2); }
    return *(const uint8_t*)s1 - pgm_read_byte(s2);
}

//returns a pointer to the first cmd argument.
char *getFirstArg(char* cmd) {
    while(*cmd && *cmd != ' ') {cmd++;}
    while(*cmd && *cmd == ' ') {cmd++;}
    return cmd;
}
