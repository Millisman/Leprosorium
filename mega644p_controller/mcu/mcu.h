#pragma once
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-attributes"

#if defined (__AVR__) || defined (__AVR_ARCH__)

#ifndef F_CPU
#define F_CPU   20000000UL
#endif

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#else

#define F_CPU   20000000UL

#define PSTR
#define SEI
#define PROGMEM
#define memcpy_P memcpy
#define strstr_P strstr
#define strcasestr_P strcasestr
#define EEMEM
#define strlen_P strlen
#include <bits/stdint-uintn.h>
#include <functional>
char pgm_read_byte(const char * _address_short);
uint8_t  pgm_read_byte(const uint8_t * _address_short);
uint16_t pgm_read_word(const uint16_t * _address_short);
uint32_t pgm_read_dword(const uint32_t * _address_short);

#define EEPROM_SIZE 2048

void _test_eeprom_reset();
void eeprom_write_block( const void * src, void* dst, size_t n );
void eeprom_read_block(  void * dest, const void * source, size_t n );

#endif

typedef enum {
    NOT_ELAPSED     = 0,
    ELAPSED_100ms   = 1,
    ELAPSED_1S      = 2
} mcu_Timer_Elapses;

typedef enum {
    LCD_4b_IDLE     = 0,
    LCD_4b_HIGH     = 2,
    LCD_4b_HIGH_W   = 3,
    LCD_4b_LOW      = 4,
    LCD_4b_LOW_W    = 5,
    LCD_4b_DONE     = 6

} mcu_Write_4b_modes;


void mcu_init_all();
void mcu_wdt_restart();

mcu_Timer_Elapses mcu_get_elapses();
uint32_t millis();
uint32_t uptime_sec();

void    mcu_lcd_insert(const uint8_t rs, const uint8_t value);
void    mcu_lcd_write4b(const uint8_t value);

