#include "nrf24.h"
#include "mcu/mcu.h"
#include "mcu/pin_macros.h"
#include "mcu/pins.h"
#include <stdio.h>

void nRF24_CE_H(void)    { ON(PIN_RF24_CE); }
void nRF24_CE_L(void)    { OFF(PIN_RF24_CE); }
void nRF24_RX_ON(void)   { ON(PIN_RF24_CE); }
void nRF24_RX_OFF(void)  { OFF(PIN_RF24_CE); }

void nRF24_CSN_L(void) {
    SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);
    OFF(PIN_RF24_CSN);
}

void nRF24_CSN_H(void) {
    ON(PIN_RF24_CSN);
}

uint8_t nRF24_LL_RW(uint8_t ch) {
    SPDR = ch;
    while(!(SPSR & (1<<SPIF)));
    uint8_t cc = SPDR;
    return cc;
}
