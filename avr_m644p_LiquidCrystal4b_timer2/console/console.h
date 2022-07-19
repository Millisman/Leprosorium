#pragma once

#include <stdint.h>

void Console_init();
void Console_update();
void Console_update_1S();
void Console_update_100ms();

void cmd_cd(char* args);
void cmd_uptime(char* args);
void cmd_reboot(char* args);
void cmd_help(char* args);
