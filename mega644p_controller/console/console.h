#pragma once
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-attributes"

void Console_init();
void Console_update();
void Console_update_1S();
void Console_update_100ms();


typedef struct {
    const char *cmd;
    const char *help;
    void (*func)(char *);
} concmd_data_t;
