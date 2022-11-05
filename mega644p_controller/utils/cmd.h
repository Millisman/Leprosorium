#pragma once

#include <stdint.h>

extern uint8_t compareCmds(const char* s1, const char* s2);

//returns a pointer to the first cmd argument.
extern char *getFirstArg(char* cmd);
