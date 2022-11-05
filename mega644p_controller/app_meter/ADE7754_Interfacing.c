#include "ADE7754.h"
#include "mcu/mcu.h"
#include "mcu/pin_macros.h"
#include "mcu/pins.h"
#include <stdio.h>
#include <util/delay.h>

uint8_t SPI_IOADE7754(uint8_t ch) {
    SPDR = ch;
    while(!(SPSR & (1<<SPIF)));
    return SPDR;
}

void ADE7754_spi_write_reg_8(uint8_t reg_address, uint8_t val) {
    OFF(PIN_CS_ADE);
    _delay_loop_1(1);
    SPI_IOADE7754(ADE7754_WRITE_REG(reg_address)); //register selection
    _delay_loop_1(1);
    SPI_IOADE7754(val);
    _delay_loop_1(1);
    ON(PIN_CS_ADE);
}

void ADE7754_spi_write_reg_16(uint8_t reg_address, uint16_t value) {
    OFF(PIN_CS_ADE);
    _delay_loop_1(1);
    SPI_IOADE7754(ADE7754_WRITE_REG(reg_address));
    _delay_loop_1(1);
    SPI_IOADE7754((value >> 8) & 0xFF); //data send, MSB first
    _delay_loop_1(1);
    SPI_IOADE7754(value & 0xFF);
    _delay_loop_1(1);
    ON(PIN_CS_ADE);
}

uint8_t ADE7754_spi_read_reg_8(uint8_t reg_address) {
    OFF(PIN_CS_ADE);
    _delay_loop_1(1);
    SPI_IOADE7754(ADE7754_READ_REG(reg_address));
    _delay_us(4);
    uint8_t b0=SPI_IOADE7754(0);
    _delay_loop_1(1);
    ON(PIN_CS_ADE);
    return b0;
}

uint16_t ADE7754_spi_read_reg_16(uint8_t reg_address) {
    OFF(PIN_CS_ADE);
    _delay_loop_1(1);
    SPI_IOADE7754(ADE7754_READ_REG(reg_address));
    _delay_us(4);
    uint8_t b1=SPI_IOADE7754(0);
    _delay_loop_1(1);
    uint8_t b0=SPI_IOADE7754(0);
    _delay_loop_1(1);
    ON(PIN_CS_ADE);
    return (uint16_t) b1<<8 | (uint16_t)b0;
}

uint32_t ADE7754_spi_read_reg_24(uint8_t reg_address) {
    OFF(PIN_CS_ADE);
    _delay_loop_1(1);
    SPI_IOADE7754(ADE7754_READ_REG(reg_address));
    _delay_us(4);
    uint8_t b2=SPI_IOADE7754(0);
    _delay_loop_1(1);
    uint8_t b1=SPI_IOADE7754(0);
    _delay_loop_1(1);
    uint8_t b0=SPI_IOADE7754(0);
    _delay_loop_1(1);
    ON(PIN_CS_ADE);
    return (uint32_t)b2<<16 | (uint32_t)b1<<8 | (uint32_t)b0;
}
