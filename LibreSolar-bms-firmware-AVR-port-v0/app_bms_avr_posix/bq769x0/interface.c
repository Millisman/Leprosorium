/*
 * Copyright (c) The Libre Solar Project Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "../config.h"
#include "../mcu.h"

#include "interface.h"
#include "registers.h"

#include <string.h>
#include "../errno-base.h"
#include "../helper.h"

int adc_gain;   // factory-calibrated, read out from chip (uV/LSB)
int adc_offset; // factory-calibrated, read out from chip (mV)

// indicates if a new current reading or an error is available from BMS IC
extern bool alert_interrupt_flag;
extern time_t alert_interrupt_timestamp;

static uint8_t i2c_address;
static bool crc_enabled;

// The bq769x0 drives the ALERT pin high if the SYS_STAT register contains
// a new value (either new CC reading or an error)
// alert_interrupt_timestamp = uptime();
// alert_interrupt_flag = true;

void bq769x0_write_byte(uint8_t reg_addr, uint8_t data)
{
    uint8_t buf[4] = { uint8_t(i2c_address << 1), // slave address incl. R/W bit required for CRC calculation
                       reg_addr, data };

    if (crc_enabled) {
        buf[3] = crc8_ccitt(0, buf, 3);
        i2c_write(buf + 1, 3, i2c_address);
    }
    else {
        i2c_write(buf + 1, 2, i2c_address);
    }
}

uint8_t bq769x0_read_byte(uint8_t reg_addr)
{
    uint8_t buf[3];

    i2c_write(&reg_addr, 1, i2c_address);

    if (crc_enabled) {
        // CRC is calculated over the slave address (incl. R/W bit) and data
        buf[0] = (i2c_address << 1) | 1U;
        do {
            if (i2c_read(buf + 1, 2, i2c_address) != 0 ) { return 0; }
        } while (crc8_ccitt(0, buf, 2) != buf[2]);

        return buf[1];
    }
    else {
        if (i2c_read(buf, 1, i2c_address) != 0 ) { return 0; }
        return buf[0];
    }
}

int32_t bq769x0_read_word(uint8_t reg_addr)
{
    uint8_t buf[5];

    // write starting register
    i2c_write(&reg_addr, 1, i2c_address);

    if (crc_enabled) {
        // CRC is calculated over the slave address (incl. R/W bit) and data
        buf[0] = (i2c_address << 1) | 1U;
        i2c_read(buf + 1, 4, i2c_address);

        if (crc8_ccitt(0, buf, 2) != buf[2]) {
            return -1;
        }

        // CRC of subsequent bytes only considering data
        if (crc8_ccitt(0, buf + 3, 1) != buf[4]) {
            return -1;
        }

        return buf[1] << 8 | buf[3];
    }
    else {
        i2c_read(buf, 2, i2c_address);
        return buf[0] << 8 | buf[1];
    }
}

// automatically find out address and CRC setting
static int determine_address_and_crc(void)
{
    i2c_address = 0x08;
    crc_enabled = true;
    bq769x0_write_byte(BQ769X0_CC_CFG, 0x19);
    if (bq769x0_read_byte(BQ769X0_CC_CFG) == 0x19) {
        return 0;
    }

    i2c_address = 0x18;
    crc_enabled = true;
    bq769x0_write_byte(BQ769X0_CC_CFG, 0x19);
    if (bq769x0_read_byte(BQ769X0_CC_CFG) == 0x19) {
        return 0;
    }

    i2c_address = 0x08;
    crc_enabled = false;
    bq769x0_write_byte(BQ769X0_CC_CFG, 0x19);
    if (bq769x0_read_byte(BQ769X0_CC_CFG) == 0x19) {
        return 0;
    }

    i2c_address = 0x18;
    crc_enabled = false;
    bq769x0_write_byte(BQ769X0_CC_CFG, 0x19);
    if (bq769x0_read_byte(BQ769X0_CC_CFG) == 0x19) {
        return 0;
    }

    return 1;
}



int bq769x0_init()
{
    alert_interrupt_flag = true; // init with true to check and clear errors at start-up
    int err = determine_address_and_crc();    
    if (err == 0) {
        // initial settings for bq769x0
        bq769x0_write_byte(BQ769X0_SYS_CTRL1, 0b00011000); // switch external thermistor and ADC on
        bq769x0_write_byte(BQ769X0_SYS_CTRL2, 0b01000000); // switch CC_EN on
        // get ADC offset and gain
        adc_offset = (signed int)bq769x0_read_byte(BQ769X0_ADCOFFSET); // 2's complement
        adc_gain = 365
                   + (((bq769x0_read_byte(BQ769X0_ADCGAIN1) & 0b00001100) << 1)
                      | ((bq769x0_read_byte(BQ769X0_ADCGAIN2) & 0b11100000) >> 5)); // uV/LSB
    }
    else {
        LOG_ERR("BMS communication error\n");
        return err;
    }

    return 0;
}

bool bq769x0_alert_flag()
{
    return alert_interrupt_flag;
}

void bq769x0_alert_flag_reset()
{
    alert_interrupt_flag = false;
}

time_t bq769x0_alert_timestamp()
{
    return alert_interrupt_timestamp;
}
