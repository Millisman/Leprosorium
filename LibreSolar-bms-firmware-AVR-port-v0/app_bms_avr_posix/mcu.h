#pragma once

#include "config.h"
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#if defined (__AVR__) || defined (__AVR_ARCH__)

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "pin_macros.h"
#include "avr/uart.h"

#define I2C_MASTER_FREQ 100000UL
#define I2C_MASTER_TOT_MS 4

#endif
// =============================================================================

#if defined (__linux) || defined (__linux__) || defined (__gnu_linux__) \
|| defined (__unix) || defined (__unix__) \
|| defined (__x86_64) || defined (__x86_64__)

#include <iostream>

#define EEMEM
#define PSTR(x) x

#define printf_P(...) printf(__VA_ARGS__)
#define eeprom_is_ready() 1
#define eeprom_busy_wait() do {} while (!eeprom_is_ready())
uint8_t eeprom_read_byte (const uint8_t *__p);
uint16_t eeprom_read_word (const uint16_t *__p);
uint32_t eeprom_read_dword (const uint32_t *__p);
float eeprom_read_float (const float *__p);
void eeprom_read_block (void *__dst, const void *__src, size_t __n);
void eeprom_write_byte (uint8_t *__p, uint8_t __value);
void eeprom_write_word (uint16_t *__p, uint16_t __value);
void eeprom_write_dword (uint32_t *__p, uint32_t __value);
void eeprom_write_float (float *__p, float __value);
void eeprom_write_block (const void *__src, void *__dst, size_t __n);
void eeprom_update_byte (uint8_t *__p, uint8_t __value);
void eeprom_update_word (uint16_t *__p, uint16_t __value);
void eeprom_update_dword (uint32_t *__p, uint32_t __value);
void eeprom_update_float (float *__p, float __value);
void eeprom_update_block (const void *__src, void *__dst, size_t __n);
#define _EEPUT(addr, val) eeprom_write_byte ((uint8_t *)(addr), (uint8_t)(val))
#define __EEPUT(addr, val) eeprom_write_byte ((uint8_t *)(addr), (uint8_t)(val))
#define _EEGET(var, addr) (var) = eeprom_read_byte ((const uint8_t *)(addr))
#define __EEGET(var, addr) (var) = eeprom_read_byte ((const uint8_t *)(addr))

#define _BV(bit) (1 << (bit))
#define bit_is_set(sfr, bit) (_SFR_BYTE(sfr) & _BV(bit))
#define bit_is_clear(sfr, bit) (!(_SFR_BYTE(sfr) & _BV(bit)))
#define loop_until_bit_is_set(sfr, bit) do { } while (bit_is_clear(sfr, bit))
#define loop_until_bit_is_clear(sfr, bit) do { } while (bit_is_set(sfr, bit))

char     pgm_read_byte(const char *_address_short);
uint8_t  pgm_read_byte(const uint8_t *_address_short);
uint16_t pgm_read_word(const uint16_t *_address_short);
uint32_t pgm_read_dword(const uint32_t *_address_short);

#endif

void i2c_master_init(void);

/**
 * @brief Write a set amount of data to an I2C device.
 *
 * This routine writes a set amount of data synchronously.
 *
 * @param dev Pointer to the device structure for an I2C controller
 * driver configured in master mode.
 * @param buf Memory pool from which the data is transferred.
 * @param num_bytes Number of bytes to write.
 * @param addr Address to the target I2C device for writing.
 *
 * @retval 0 If successful.
 * @retval -EIO General input / output error.
 */
int8_t i2c_write(const uint8_t *buf, uint8_t num_bytes, uint8_t addr);

/**
 * @brief Read a set amount of data from an I2C device.
 *
 * This routine reads a set amount of data synchronously.
 *
 * @param dev Pointer to the device structure for an I2C controller
 * driver configured in master mode.
 * @param buf Memory pool that stores the retrieved data.
 * @param num_bytes Number of bytes to read.
 * @param addr Address of the I2C device being read.
 *
 * @retval 0 If successful.
 * @retval -EIO General input / output error.
 */
int8_t i2c_read(uint8_t *buf, uint8_t num_bytes, uint8_t addr);


void mcu_init();
uint32_t millis2(); // timer ms counter
uint32_t uptime2(); // uptime sec counter
bool elapsed_250ms();
bool elapsed_one_sec();

/**
 * Init port
 */
void leds_init();

/**
 * Set green charging LED on or off
 */
void leds_chg_set(bool on);

/**
 * Set red discharging LED on or off
 */
void leds_dis_set(bool on);
