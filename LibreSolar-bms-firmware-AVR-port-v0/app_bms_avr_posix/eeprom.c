/*
 * Copyright (c) The Libre Solar Project Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mcu.h"
#include "eeprom.h"

#define DATA_OBJECTS_VERSION    88
#define EEPROM_HEADER_SIZE      8 // bytes

// K_MUTEX_DEFINE(eeprom_buf_lock);
// Buffer used by store and restore functions (must be word-aligned for hardware CRC calculation)
// static uint8_t buf[512];// __aligned(sizeof(uint32_t));
// extern ThingSet ts;

uint32_t _calc_crc(const uint8_t *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0; //CRC->DR;
}

typedef struct __attribute__((__packed__))
{
    struct
    {
        uint16_t version;   // bytes 0-1: Version number
        uint16_t len;       // bytes 2-3: number of data bytes
        uint32_t crc;       // bytes 4-7: CRC32
    } header;
    uint8_t buf[512];       // byte 8: start of data
} EEPROMDataStruct;

EEPROMDataStruct    EEMEM  EE_DataStruct;
static EEPROMDataStruct    buf;

void eeprom_restore_data() {
    eeprom_read_block(&buf.header, &EE_DataStruct.header, sizeof(buf.header));
    printf_P(PSTR("EEPROM header restore: ver %d, len %d, CRC %.8x\n"),
           buf.header.version, buf.header.len, (unsigned int)buf.header.crc);
    
    if (buf.header.version == DATA_OBJECTS_VERSION && buf.header.len <= sizeof(buf.buf)) {
        eeprom_read_block(&buf.buf, &EE_DataStruct.buf, sizeof(buf.buf));

        printf_P(PSTR("Data (len=%u): "), buf.header.len);
        for (uint16_t i = 0; i < buf.header.len; i++) printf_P(PSTR("%.2x "), buf.buf[i]);

        if (_calc_crc(buf.buf, buf.header.len) == buf.header.crc) {
            int status = 0; //ts.bin_import(buf, sizeof(buf), TS_WRITE_MASK, SUBSET_NVM);
            printf_P(PSTR("EEPROM: Data objects read and updated, ThingSet result: 0x%x\n"), status);
        }
        else {
            printf_P(PSTR("EEPROM: CRC of data not correct, expected 0x%x (data_len = %d)\n"),
                   (unsigned int)buf.header.crc, buf.header.len);
        }
    }
    else {
        printf_P(PSTR("EEPROM: Empty or data layout version changed\n"));
    }
}

void eeprom_store_data()
{
    //buf.header.len = ts.bin_export(buf + EEPROM_HEADER_SIZE, sizeof(buf) - EEPROM_HEADER_SIZE, SUBSET_NVM);
    buf.header.len = 77;
    buf.header.version = DATA_OBJECTS_VERSION;
    buf.header.crc = _calc_crc(buf.buf, buf.header.len);
    
    printf_P(PSTR("Data (len=%d): "), buf.header.len);
    for (uint16_t i = 0; i < buf.header.len; i++) printf_P(PSTR("%.2x "), buf.buf[i]);

    // printf_P(PSTR("Header: %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
    //     buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);

    if (buf.header.len == 0) {
        printf_P(PSTR("EEPROM: Data could not be stored. ThingSet error (len = %d)\n"), buf.header.len);
    }
    else {
        
//         eeprom_read_block(&buf.buf, &EE_DataStruct.buf, sizeof(buf.buf));
//         DeviceSetup_Block.crc8 = utils::gencrc8((uint8_t *)&DeviceSetup_Block, sizeof(DeviceSetup_Block)-1);
        eeprom_write_block(&buf.buf, &EE_DataStruct.buf, sizeof(EE_DataStruct.buf));
        
//         eeprom_update_byte
        
        
//         eeprom_write(0, buf, len + EEPROM_HEADER_SIZE);
//         if (err == 0) {
//             printf_P(PSTR("EEPROM: Data successfully stored.\n");
//         }
//         else {
//             printf_P(PSTR("EEPROM: Write error.\n");
//         }
    }
    //k_mutex_unlock(&eeprom_buf_lock);
}

// void eeprom_update() 
// {
//     // uptime, sec
//     uint32_t uptime = k_uptime_get() / 1000; // Get system uptime. This routine returns the elapsed time since the system booted, in milliseconds.
//     if (uptime % EEPROM_UPDATE_INTERVAL == 0 && uptime > 0) {
//         eeprom_store_data();
//     }
// }

// #else
// void eeprom_store_data()
// {
//     ;
// }
// void eeprom_restore_data()
// {
//     ;
// }
// void eeprom_update()
// {
//     ;
// }
// #endif
