#pragma once
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-attributes"

#include <stdint.h>

void app_Radio_init();
void app_Radio_update();
void app_Radio_update_1s();
void app_Radio_update_100ms();

typedef struct __attribute__((__packed__)) {
    int16_t L1_Vrms;
    int32_t L1_Irms;
    int32_t L1_apparentPower;
    int16_t L2_Vrms;
    int32_t L2_Irms;
    int32_t L2_apparentPower;
    int16_t L3_Vrms;
    int32_t L3_Irms;
    int32_t L3_apparentPower;
    uint8_t PEN_Vrms;
    int8_t Rtemp;
} RF_data_t;
