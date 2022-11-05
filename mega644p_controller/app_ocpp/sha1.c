#include <string.h>
#include "sha1.h"
#include "mcu/mcu.h"

#define HMAC_IPAD 0x36
#define HMAC_OPAD 0x5c
#define SHA1_K0  0x5a827999
#define SHA1_K20 0x6ed9eba1
#define SHA1_K40 0x8f1bbcdc
#define SHA1_K60 0xca62c1d6

const uint8_t sha1InitState[] PROGMEM = {
    0x01,0x23,0x45,0x67, // H0
    0x89,0xab,0xcd,0xef, // H1
    0xfe,0xdc,0xba,0x98, // H2
    0x76,0x54,0x32,0x10, // H3
    0xf0,0xe1,0xd2,0xc3  // H4
};

static sha1_mem *mem;

void Sha1_init_173(sha1_mem *m) {
    mem = m;
    memset(mem, 0, sizeof(sha1_mem));
    memcpy_P(mem->state.b, sha1InitState, HASH_LENGTH);
    mem->byteCount = 0;
    mem->bufferOffset = 0;
}

uint32_t Sha1_rol32(uint32_t number, uint8_t bits) {
    return ((number << bits) | (number >> (32-bits)));
}

void Sha1_hashBlock() {
    uint8_t     i;
    uint32_t    a = mem->state.w[0];
    uint32_t    b = mem->state.w[1];
    uint32_t    c = mem->state.w[2];
    uint32_t    d = mem->state.w[3];
    uint32_t    e = mem->state.w[4];
    uint32_t    t;
    for (i = 0; i<80; i++) {
        if (i>=16) {
            t = mem->buffer.w[(i+13)&15] ^ mem->buffer.w[(i+8)&15] ^ mem->buffer.w[(i+2)&15] ^ mem->buffer.w[i&15];
            mem->buffer.w[i&15] = Sha1_rol32(t,1);
        }
        if (i<20) {
            t = (d ^ (b & (c ^ d))) + SHA1_K0;
        } else if (i<40) {
            t = (b ^ c ^ d) + SHA1_K20;
        } else if (i<60) {
            t = ((b & c) | (d & (b | c))) + SHA1_K40;
        } else {
            t = (b ^ c ^ d) + SHA1_K60;
        }
        t += Sha1_rol32(a,5) + e + mem->buffer.w[i&15];
        e=d;
        d=c;
        c= Sha1_rol32(b,30);
        b=a;
        a=t;
    }
    mem->state.w[0] += a;
    mem->state.w[1] += b;
    mem->state.w[2] += c;
    mem->state.w[3] += d;
    mem->state.w[4] += e;
}

void Sha1_addUncounted(uint8_t data) {
    mem->buffer.b[mem->bufferOffset ^ 3] = data;
    mem->bufferOffset++;
    if (mem->bufferOffset == BLOCK_LENGTH) {
        Sha1_hashBlock();
        mem->bufferOffset = 0;
    }
}

size_t Sha1_write(uint8_t data) {
    ++mem->byteCount;
    Sha1_addUncounted(data);
    return sizeof(data);
}

void Sha1_pad() {
    Sha1_addUncounted(0x80);
    while (mem->bufferOffset != 56) Sha1_addUncounted(0x00);
    Sha1_addUncounted(0);
    Sha1_addUncounted(0);
    Sha1_addUncounted(0);
    Sha1_addUncounted(mem->byteCount >> 29);
    Sha1_addUncounted(mem->byteCount >> 21);
    Sha1_addUncounted(mem->byteCount >> 13);
    Sha1_addUncounted(mem->byteCount >> 5);
    Sha1_addUncounted(mem->byteCount << 3);
}


uint8_t* Sha1_result(void) {
    Sha1_pad();
    for (uint8_t i = 0; i<5; i++) {
        uint32_t a,b;
        a = mem->state.w[i];
        b=a<<24;
        b|=(a<<8) & 0x00ff0000;
        b|=(a>>8) & 0x0000ff00;
        b|=a>>24;
        mem->state.w[i]=b;
    }
    return mem->state.b;
}
