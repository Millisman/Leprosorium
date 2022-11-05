#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <string.h>
#include "mcu/mcu.h"
#include "ADE7758.h"
#include "meter.h"

void app_Meter_init() {
    ADE7758_InitialSetup();
    printf(" ADE7758_getCHKSUM %u\n", ADE7758_getCHKSUM());
    printf("ADE7758_getVersion %u\n", ADE7758_getVersion());
    printf("   ADE7758_getTEMP %d\n", ADE7758_getTEMP());
    printf(" ADE7758_getOpMode %u\n", ADE7758_getOpMode());
}

void app_Meter_update() {
    
}

void app_Meter_update_1s() {
    
}

void app_Meter_update_100ms() {

}

// // // // // // // // // void ADE7758_InitialSetup() {
// // // // // // // // //     
// // // // // // // // // }


// static      uint8_t    _BufTag[MFRC522_MAX_LEN];
// volatile    uid_data_t _TmpTag;
// volatile    uid_data_t LastTag;
// volatile    uint32_t   LastTag_ts;
// 
// void app_CardReader_update() {}
// void app_CardReader_update_1s() {}
// void app_CardReader_update_100ms() {

//     uint8_t status = MFRC522_Request(PICC_REQIDL, _BufTag);
//     if (status == MI_OK) {
//         _TmpTag.len = _BufTag[0];
//         _TmpTag.sak = _BufTag[1];
//         printf_P(PSTR("Len=%02u Sak=0x%02X UID=0x"), _TmpTag.len, _TmpTag.sak);
//         status = MFRC522_Anticoll(_BufTag, _TmpTag.len);
//         if (status == MI_OK) {
//             for (status = 0; status < _TmpTag.len; ++status) {
//                 if (status < sizeof(_TmpTag.uid)) {
//                     _TmpTag.uid[status] = _BufTag[status];
//                     printf_P(PSTR("%02X"), _BufTag[status]);
//                 }
//             }
//             LastTag_ts = uptime_sec();
//             if (memcmp((void *)&_TmpTag, (void *)&LastTag, _TmpTag.len + 2) == 0) {
//                 printf_P(PSTR(" [OLD"));
//             } else {
//                 memcpy((void *)&LastTag, (void *)&_TmpTag, sizeof(LastTag));
//                  printf_P(PSTR(" [NEW"));
//             }
//             printf_P(PSTR(" %lu]\n"), LastTag_ts);
//         } else {
//             memset((void *)&_TmpTag, 0, sizeof(_TmpTag));
//         }
//     }
// }

/*
void app_CardReader_init() {
    MFRC522_Reset();
    MFRC522_WriteRegister(MFRC522_REG_T_MODE,       0x8D);
    MFRC522_WriteRegister(MFRC522_REG_T_PRESCALER,  0x3E);
    MFRC522_WriteRegister(MFRC522_REG_T_RELOAD_L,   30);
    MFRC522_WriteRegister(MFRC522_REG_T_RELOAD_H,   0);
    MFRC522_WriteRegister(MFRC522_REG_RF_CFG,       0x70); // 48dB gain
    MFRC522_WriteRegister(MFRC522_REG_TX_AUTO,      0x40);
    MFRC522_WriteRegister(MFRC522_REG_MODE,         0x3D);
    MFRC522_AntennaOn(); // Open the antenna
}
*/
