#pragma once
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-attributes"

#include "RC522.h"

typedef struct {
    uint8_t     sak;
    uint8_t     len;
    uint8_t     uid[10];
} uid_data_t;

void app_CardReader_init();
void app_CardReader_update();
void app_CardReader_update_1s();
void app_CardReader_update_100ms();
