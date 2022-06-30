#include "mcu.h"
/*
    touch myfile.c
    gcc -std=c99 -E -dM myfile.c
*/

#if defined (__linux) || defined (__linux__) || defined (__gnu_linux__) \
    || defined (__unix) || defined (__unix__) \
    || defined (__x86_64) || defined (__x86_64__) \

// -------------------------- mocking mode ------------------------

#include <sys/time.h>



uint64_t current_ms_stamp() {
    struct timeval te;    
    gettimeofday(&te, NULL); // get current time
    return te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds;
}

static uint64_t app_start_ms = 0;

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

int8_t i2c_write(const uint8_t *buf, uint32_t num_bytes, uint16_t addr)
{
    printf("\ni2c_write(buf=%s, uint32_t num_bytes=%u, uint16_t addr=%u)\n", buf, num_bytes, addr);
    return 0; // 0 - successful, or -EIO General - input / output error.
}

int8_t i2c_read(uint8_t *buf, uint32_t num_bytes, uint16_t addr)
{
    printf("\ni2c_read(buf=%s, uint32_t num_bytes=%u, uint16_t addr=%u)\n", buf, num_bytes, addr);
    return 0;
}

char     pgm_read_byte(const char *_address_short)      {return *_address_short;}
uint8_t  pgm_read_byte(const uint8_t *_address_short)   {return *_address_short;}
uint16_t pgm_read_word(const uint16_t *_address_short)  {return *_address_short;}
uint32_t pgm_read_dword(const uint32_t *_address_short) {return *_address_short;}


uint8_t eeprom_read_byte (const uint8_t *__p)                           { printf("eeprom_read_byte\n"); }
uint16_t eeprom_read_word (const uint16_t *__p)                         { printf("eeprom_read_word\n"); }
uint32_t eeprom_read_dword (const uint32_t *__p)                        { printf("eeprom_read_dword\n"); }
float eeprom_read_float (const float *__p)                              { printf("eeprom_read_float\n"); }
void eeprom_read_block (void *__dst, const void *__src, size_t __n)     { printf("eeprom_read_block\n"); }
void eeprom_write_byte (uint8_t *__p, uint8_t __value)                  { printf("eeprom_write_byte\n"); }
void eeprom_write_word (uint16_t *__p, uint16_t __value)                { printf("eeprom_write_word\n"); }
void eeprom_write_dword (uint32_t *__p, uint32_t __value)               { printf("eeprom_write_dword\n"); }
void eeprom_write_float (float *__p, float __value)                     { printf("eeprom_write_float\n"); }
void eeprom_write_block (const void *__src, void *__dst, size_t __n)    { printf("eeprom_write_block\n"); }
void eeprom_update_byte (uint8_t *__p, uint8_t __value)                 { printf("eeprom_update_byte\n"); }
void eeprom_update_word (uint16_t *__p, uint16_t __value)               { printf("eeprom_update_word\n"); }
void eeprom_update_dword (uint32_t *__p, uint32_t __value)              { printf("eeprom_update_dword\n"); }
void eeprom_update_float (float *__p, float __value)                    { printf("eeprom_update_float\n"); }
void eeprom_update_block (const void *__src, void *__dst, size_t __n)   { printf("eeprom_update_block\n"); }

void mcu_init() {
    printf("mcu_init\n");
}

void twi_master_init() {
        printf("twi_master_init\n");
}


#endif
