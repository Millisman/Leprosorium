/*
 * Copyright (c) The Libre Solar Project Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "helper.h"
#include "mcu.h"

float interpolate(const float a[], const float b[], size_t size, float value_a)
{
    if (a[0] < a[size - 1]) {
        for (unsigned int i = 0; i < size; i++) {
            if (value_a <= a[i]) {
                if (i == 0) {
                    return b[0]; // value_a smaller than first element
                }
                else {
                    return b[i - 1] + (b[i] - b[i - 1]) * (value_a - a[i - 1]) / (a[i] - a[i - 1]);
                }
            }
        }
        return b[size - 1]; // value_a larger than last element
    }
    else {
        for (unsigned int i = 0; i < size; i++) {
            if (value_a >= a[i]) {
                if (i == 0) {
                    return b[0]; // value_a smaller than first element
                }
                else {
                    return b[i - 1] + (b[i] - b[i - 1]) * (value_a - a[i - 1]) / (a[i] - a[i - 1]);
                }
            }
        }
        return b[size - 1]; // value_a larger than last element
    }
}

const char *byte2bitstr(uint8_t b)
{
    static char str[9];

    str[0] = '\0';
    for (int z = 128; z > 0; z >>= 1) {
        strcat(str, ((b & z) == z) ? "1" : "0");
    }

    return str;
}


const static uint8_t crc8_ccitt_small_table[16] = {
    0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15,
    0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d
};

uint8_t crc8_ccitt(uint8_t val, const void *buf, size_t cnt)
{
    size_t i;
    const uint8_t *p = (const uint8_t *)buf;
    
    for (i = 0; i < cnt; i++) {
        val ^= p[i];
        val = (val << 4) ^ crc8_ccitt_small_table[val >> 4];
        val = (val << 4) ^ crc8_ccitt_small_table[val >> 4];
    }
    return val;
}


/*
uint8_t _crc8_ccitt_update (uint8_t inCrc, uint8_t inData) {
    uint8_t data = inCrc ^ inData;
    for (uint8_t i = 0; i < 8; i++) {
        if ((data & 0x80) != 0) {
            data <<= 1;
            data ^= 0x07;
        } else {
            data <<= 1;
        }
    }
    return data;
}


#ifdef BQ769X0_CRC_ENABLED
// CRC is calculated over the slave address (including R/W bit), register address, and data.
i2buf[2] = _crc8_ccitt_update(0, (BQ769X0_I2C_ADDR << 1) | 0);
i2buf[2] = _crc8_ccitt_update(i2buf[2], address);
i2buf[2] = _crc8_ccitt_update(i2buf[2], data);
Wire.write(BQ769X0_I2C_ADDR, i2buf, 3);
#else
Wire.write(BQ769X0_I2C_ADDR, i2buf, 2);
#endif

*/

