#pragma once
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-attributes"

#include "mcu/mcu.h"



typedef struct __attribute__((__packed__)) {
    uint32_t    SerialNumber;
    uint32_t    Assembled;
    uint8_t     DeviceType;
    uint8_t     DeviceModel;
    uint8_t     HW_Revision;
    uint8_t     SW_Revision;
    uint8_t     crc8;
} Device_SN_sector_t;


typedef struct __attribute__((__packed__)) {
    Device_SN_sector_t  Serial_Sector;
    uint8_t             blob_reserve[512];
} Full_EEPROM_Sector_t;


void app_Store_init();
void app_Store_update_1s();
// s_DeviceSetup_Block EEMEM   EEM_DeviceSetup_Block;


// void EEPROM_store_update_1s();
// void EEPROM_store_init();
