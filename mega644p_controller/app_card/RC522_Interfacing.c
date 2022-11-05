#include "RC522.h"
#include "mcu/mcu.h"
#include "mcu/pin_macros.h"
#include "mcu/pins.h"
#include <stdio.h>
#include <util/delay.h>

uint8_t SPI_IO522(uint8_t ch) {
    SPDR = ch;
    while(!(SPSR & _BV(SPIF)));
    return SPDR;
}

void MFRC522_WriteRegister(uint8_t addr, uint8_t val) {
    SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);
    OFF(PIN_CS_NFC);
    SPI_IO522((addr << 1) & 0x7E); // Address format: 0XXXXXX0
    SPI_IO522(val);
    SPCR = 0;
    ON(PIN_CS_NFC);
}

uint8_t MFRC522_ReadRegister(uint8_t addr) {
    SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);
    OFF(PIN_CS_NFC);
    SPI_IO522(((addr << 1) & 0x7E) | 0x80);
    uint8_t ret = SPI_IO522(0);
    SPCR = 0;
    ON(PIN_CS_NFC);
    return ret;
}
