#include "spimaster.h"
#include "pins.h"

#define SPI_MODE_MASK       0x0C    // CPOL = bit 3, CPHA = bit 2 on SPCR
#define SPI_CLOCK_MASK      0x03    // SPR1 = bit 1, SPR0 = bit 0 on SPCR
#define SPI_2XCLOCK_MASK    0x01    // SPI2X = bit 0 on SPSR

void SPI_Master_Init(SPI_regs_t *spi_reg, SPI_dataMode dataMode, SPI_clockDiv clockDiv, SPI_bitOrder bitOrder) {
    // Invert the SPI2X bit
    uint8_t cdiv = clockDiv;
    cdiv ^= 0x1;
    // Pack into the SPISettings class
    spi_reg->reg_SPCR = _BV(SPE) | _BV(MSTR) | ((bitOrder == LSBFIRST) ? _BV(DORD) : 0) |
    (dataMode & SPI_MODE_MASK) | ((cdiv >> 1) & SPI_CLOCK_MASK);
    spi_reg->reg_SPSR = cdiv & SPI_2XCLOCK_MASK;
}

uint8_t SPI_Master_Transfer8_TxRx(uint8_t data) {
    SPDR = data;
    while (!(SPSR & _BV(SPIF)));
    return SPDR;
}

uint8_t SPI_Master_Transfer8_Rx() {
    SPDR = 0;
    while (!(SPSR & _BV(SPIF)));
    return SPDR;
}

void SPI_Master_Transfer_Tx(void *buf, uint8_t count) {
    if (count == 0) return;
    uint8_t *p = (uint8_t *)buf;
    SPDR = *p;
    while (--count > 0) {
        uint8_t out = *(p + 1);
        while (!(SPSR & _BV(SPIF)));
        uint8_t in = SPDR;
        SPDR = out;
        *p++ = in;
    }
    while (!(SPSR & _BV(SPIF)));
    *p = SPDR;
}

uint16_t SPI_Master_Transfer16(uint16_t data) {
    union { uint16_t val; struct { uint8_t lsb; uint8_t msb; }; } in, out;
    in.val = data;
    if (!(SPCR & _BV(DORD))) {
        SPDR = in.msb;
        while (!(SPSR & _BV(SPIF)));
        out.msb = SPDR;
        SPDR = in.lsb;
        while (!(SPSR & _BV(SPIF)));
        out.lsb = SPDR;
    } else {
        SPDR = in.lsb;
        while (!(SPSR & _BV(SPIF)));
        out.lsb = SPDR;
        SPDR = in.msb;
        while (!(SPSR & _BV(SPIF)));
        out.msb = SPDR;
    }
    return out.val;
}
