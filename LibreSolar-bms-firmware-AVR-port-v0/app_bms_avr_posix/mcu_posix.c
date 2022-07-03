#include "mcu.h"

// gcc -std=c99 -E -dM myfile.c

#if defined (__AVR__) || defined (__AVR_ARCH__)

#pragma message "mcu_posix.c - skipped in __AVR_ARCH__"

#else

// mocking mode

#include <sys/time.h>

static uint64_t app_start_ms = 0;

uint64_t current_ms_stamp() {
    struct timeval te;    
    gettimeofday(&te, NULL); // get current time
    return te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds;
}

uint32_t millis2() {
    if (app_start_ms == 0)
    {
        app_start_ms = current_ms_stamp();
        printf("\nmillis2=0 (first call)\n");
        return 0;
    } else {
        return current_ms_stamp() - app_start_ms;
    }
}

uint32_t uptime2() {
    return (current_ms_stamp() - app_start_ms) / 1000;
}


char     pgm_read_byte(const char *_address_short)      {return *_address_short;}
uint8_t  pgm_read_byte(const uint8_t *_address_short)   {return *_address_short;}
uint16_t pgm_read_word(const uint16_t *_address_short)  {return *_address_short;}
uint32_t pgm_read_dword(const uint32_t *_address_short) {return *_address_short;}


uint8_t eeprom_read_byte (const uint8_t *__p)                           { (void)__p; printf("eeprom_read_byte\n"); return 0; }
uint16_t eeprom_read_word (const uint16_t *__p)                         { (void)__p; printf("eeprom_read_word\n"); return 0; }
uint32_t eeprom_read_dword (const uint32_t *__p)                        { (void)__p; printf("eeprom_read_dword\n"); return 0; }
float eeprom_read_float (const float *__p)                              { (void)__p; printf("eeprom_read_float\n"); return 0.0f; }
void eeprom_read_block (void *__dst, const void *__src, size_t __n)     { (void)__dst; (void)__src; (void)__n; printf("eeprom_read_block\n"); }
void eeprom_write_byte (uint8_t *__p, uint8_t __value)                  { (void)__p; (void)__value; printf("eeprom_write_byte\n"); }
void eeprom_write_word (uint16_t *__p, uint16_t __value)                { (void)__p; (void)__value; printf("eeprom_write_word\n"); }
void eeprom_write_dword (uint32_t *__p, uint32_t __value)               { (void)__p; (void)__value; printf("eeprom_write_dword\n"); }
void eeprom_write_float (float *__p, float __value)                     { (void)__p; (void)__value; printf("eeprom_write_float\n"); }
void eeprom_write_block (const void *__src, void *__dst, size_t __n)    { (void)__src; (void)__dst; (void)__n; printf("eeprom_write_block\n"); }
void eeprom_update_byte (uint8_t *__p, uint8_t __value)                 { (void)__p; (void)__value; printf("eeprom_update_byte\n"); }
void eeprom_update_word (uint16_t *__p, uint16_t __value)               { (void)__p; (void)__value; printf("eeprom_update_word\n"); }
void eeprom_update_dword (uint32_t *__p, uint32_t __value)              { (void)__p; (void)__value; printf("eeprom_update_dword\n"); }
void eeprom_update_float (float *__p, float __value)                    { (void)__p; (void)__value; printf("eeprom_update_float\n"); }
void eeprom_update_block (const void *__src, void *__dst, size_t __n)   { (void)__src; (void)__dst; (void)__n; printf("eeprom_update_block\n"); }

void mcu_init() { printf("mcu_init()\n"); }

void i2c_master_init() { printf("i2c_master_init()\n"); }

int8_t i2c_write(const uint8_t *buf, uint8_t num_bytes, uint8_t addr) {    
    printf("i2c_write(num_bytes=%u, addr=0x%.2X)\t", num_bytes, addr);
    for (int i = 0; i < num_bytes; i++) printf_P(PSTR("0x%.2X "), buf[i]);
    printf(PSTR("\n"));
    return 0; // 0 - successful, or -EIO General - input / output error.
}

int8_t i2c_read(uint8_t *buf, uint8_t num_bytes, uint8_t addr) {
    (void)buf;
    (void)num_bytes;
    (void)addr;
    printf("i2c_read(num_bytes=%u, uint16_t addr=0x%.2X)\n", num_bytes, addr);
    return 1;
}

void leds_init() { printf("leds_init()\n"); }
void leds_chg_set(bool on) { printf("leds_chg_set( %u )\n", on); }
void leds_dis_set(bool on) { printf("leds_dis_set( %u )\n", on); }


#endif
