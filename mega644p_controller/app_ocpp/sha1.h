#pragma once
#include "mcu/mcu.h"

#ifdef __AVR__
#include <avr/pgmspace.h>
#else
#define PROGMEM
#define memcpy_P memcpy
#endif

#define HASH_LENGTH     20
#define BLOCK_LENGTH    64

typedef struct __attribute__((__packed__)) {
    union {
        uint8_t     b[BLOCK_LENGTH];
        uint32_t    w[BLOCK_LENGTH/4];
    } buffer;
    union {
        uint8_t     b[HASH_LENGTH];
        uint32_t    w[HASH_LENGTH/4];
    } state;
    uint8_t keyBuffer[BLOCK_LENGTH];
    uint8_t innerHash[HASH_LENGTH];
    uint8_t bufferOffset;
    uint32_t byteCount;
} sha1_mem;



void Sha1_pad();
void Sha1_addUncounted(uint8_t data);
void Sha1_hashBlock();
uint32_t Sha1_rol32(uint32_t number, uint8_t bits);
void Sha1_init_173(sha1_mem *m);
// void Sha1_initHmac(const uint8_t* secret, int secretLength);
uint8_t *Sha1_result(void);
// uint8_t *Sha1_resultHmac(void);
size_t Sha1_write(uint8_t);
