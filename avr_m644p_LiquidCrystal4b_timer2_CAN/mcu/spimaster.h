#pragma once

#include <stdint.h>
#include <avr/io.h>

typedef enum: uint8_t {
    LSBFIRST    = 0,
    MSBFIRST    = 1
} SPI_bitOrder;

typedef enum: uint8_t {
    SPI_CLOCK_DIV4      =   0x00,
    SPI_CLOCK_DIV16     =   0x01,
    SPI_CLOCK_DIV64     =   0x02,
    SPI_CLOCK_DIV128    =   0x03,
    SPI_CLOCK_DIV2      =   0x04,
    SPI_CLOCK_DIV8      =   0x05,
    SPI_CLOCK_DIV32     =   0x06
} SPI_clockDiv;

typedef enum: uint8_t {
    SPI_MODE0   =   0x00,
    SPI_MODE1   =   0x04,
    SPI_MODE2   =   0x08,
    SPI_MODE3   =   0x0C
} SPI_dataMode;



typedef struct {
    uint8_t reg_SPCR;
    uint8_t reg_SPSR;
} SPI_regs_t;


void SPI_Master_Init(SPI_regs_t *spi_reg, SPI_dataMode dataMode, SPI_clockDiv clockDiv, SPI_bitOrder bitOrder);

#define SPI_Master_StartTransmission(x, OFF_MACRO) {   \
    SPSR = x.reg_SPSR;                          \
    SPCR = x.reg_SPCR;                          \
    OFF_MACRO;                                  \
}                                               \

#define SPI_Master_EndTransmission(ON_MACRO) { \
    SPCR &= ~(_BV(SPE));                \
    ON_MACRO;                           \
}                                       \

uint8_t SPI_Master_Transfer8_TxRx(uint8_t data);
uint8_t SPI_Master_Transfer8_Rx();
void SPI_Master_Transfer_Tx(void *buf, uint8_t count);
uint16_t SPI_Master_Transfer16(uint16_t data);
