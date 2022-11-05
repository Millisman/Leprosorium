#include "eeprom_store.h"

#include <stdio.h>

Full_EEPROM_Sector_t EEMEM _EEM_BLOB;

volatile Device_SN_sector_t _Dev_SN_Struct;

volatile uint16_t _eeprom_save_interval = 360; // --1h

void app_Store_init() {
    printf_P(PSTR("EEPROM_store_init\n"));

    // // //     if (o == SetCMD::S_READ) {
    // //         eeprom_read_block((void *)&_Dev_SN_Struct, &_EEM_BLOB.Serial_Sector, sizeof(_EEM_BLOB.Serial_Sector));
    // //         if (_Dev_SN_Struct.crc8 != gencrc8((uint8_t *)&DeviceSetup_Block, sizeof(DeviceSetup_Block)-1)) {o = SetCMD::S_DEFS;}
    // //     }
    // //     if (o == SetCMD::S_DEFS) {
    // //         DeviceSetup_Block.auth              = AuthType::NOT_NEED;  // 0 - not used, or used
    // //         DeviceSetup_Block.amperageMax       = 40; // <= 80 A
    // //         DeviceSetup_Block.amperage          = 32; // < amperageMax
    // //         DeviceSetup_Block.gsm_mode          = 1;  // 0 - GSM disabled...
    // //         DeviceSetup_Block.fall_back_pp      = 6;  // 0 - use standart tabe, 6-80 if not detect
    // //         DeviceSetup_Block.sn                = 0;  // hardware serial number
    // //         o = SetCMD::S_WRITE;
    // //     }
    // //     if (o == SetCMD::S_WRITE) {
    // //         DeviceSetup_Block.ts = TimeUnix_Block.ts;
    // //         DeviceSetup_Block.crc8 = utils::gencrc8((uint8_t *)&DeviceSetup_Block, sizeof(DeviceSetup_Block)-1);
    // //         eeprom_write_block(&DeviceSetup_Block, &EEM_DeviceSetup_Block, sizeof(DeviceSetup_Block));
    // //     }
}

void app_Store_update_1s() {
    if (_eeprom_save_interval) {
        --_eeprom_save_interval;
//         printf_P(PSTR("save_interval down counter: %u\n"), _eeprom_save_interval);
    } else {
        printf_P(PSTR("save_interval sheduled!\n"));
        _eeprom_save_interval = 360;
    }
}

// void EEPROM_store_update_1s() {
//
//
// }


// void EEPROM_store_init() {
//
//
//
//
// }
