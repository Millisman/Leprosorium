#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <string.h>

#include "card_reader.h"
#include "mcu/mcu.h"

volatile    uid_data_t _TmpTag;
volatile    uid_data_t LastTag;
volatile    uint32_t   LastTag_ts;

void app_CardReader_update() {}
void app_CardReader_update_1s() {}

void app_CardReader_update_100ms() {

    if (MFRC522_Request(PICC_REQIDL) == MI_OK) {
        uint8_t *b = MFRC522_GetBuff();

        _TmpTag.len = b[0];
        _TmpTag.sak = b[1];

        if (MFRC522_Anticoll() == MI_OK) {

            printf_P(PSTR("Len=%02u Sak=0x%02X UID=0x"), _TmpTag.len, _TmpTag.sak);

            for (uint8_t x = 0; x < sizeof(_TmpTag.uid); ++x) {
                _TmpTag.uid[x] = b[x];
                printf_P(PSTR("%02X"), _TmpTag.uid[x]);
            }

            LastTag_ts = uptime_sec();

            if (memcmp((void *)&_TmpTag, (void *)&LastTag, _TmpTag.len + 2) == 0) {
                printf_P(PSTR(" [OLD"));
            } else {
                memcpy((void *)&LastTag, (void *)&_TmpTag, sizeof(LastTag));
                 printf_P(PSTR(" [NEW"));
            }

            printf_P(PSTR(" %lu]\n"), LastTag_ts);

        } else {
            memset((void *)&_TmpTag, 0, sizeof(_TmpTag));
        }
    }
}


void app_CardReader_init() {
    MFRC522_Reset();
    _delay_ms(1);
    MFRC522_WriteRegister(MFRC522_REG_T_MODE,       0x8D);
    MFRC522_WriteRegister(MFRC522_REG_T_PRESCALER,  0x3E);
    MFRC522_WriteRegister(MFRC522_REG_T_RELOAD_L,   30);
    MFRC522_WriteRegister(MFRC522_REG_T_RELOAD_H,   0);
    MFRC522_WriteRegister(MFRC522_REG_RF_CFG,       0x70); // 48dB gain
    MFRC522_WriteRegister(MFRC522_REG_TX_AUTO,      0x40);
    MFRC522_WriteRegister(MFRC522_REG_MODE,         0x3D);
    MFRC522_AntennaOn(); // Open the antenna
}

