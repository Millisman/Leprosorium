/*
 * Copyright (c) The Libre Solar Project Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "config.h"
#include "mcu.h"
#include "leds.h"
#include "bms.h"

extern Bms bms;

#ifndef UNIT_TEST


void leds_chg_set(bool on)
{
#if defined (__AVR__) || defined (__AVR_ARCH__)
    if (on) { ON(PIN_LED_CHG); } else {OFF(PIN_LED_CHG);  }
#endif
#if defined (__linux) || defined (__linux__) || defined (__gnu_linux__) \
|| defined (__unix) || defined (__unix__) \
|| defined (__x86_64) || defined (__x86_64__)
    printf("leds_chg_set=%u", on);
#endif
}

void leds_dis_set(bool on)
{
#if defined (__AVR__) || defined (__AVR_ARCH__)
    if (on) { ON(PIN_LED_DIS); } else {OFF(PIN_LED_DIS);  }
#endif

#if defined (__linux) || defined (__linux__) || defined (__gnu_linux__) \
|| defined (__unix) || defined (__unix__) \
|| defined (__x86_64) || defined (__x86_64__)
    printf("leds_dis_set=%u", on);
#endif
}

void leds_init()
{
#if defined (__AVR__) || defined (__AVR_ARCH__)
    DRIVER(PIN_LED_CHG, OUT);
    DRIVER(PIN_LED_DIS, OUT);
    OFF(PIN_LED_CHG);
    OFF(PIN_LED_DIS);
#endif 
}

void leds_update()
{
    static uint32_t count = 0;

    // Charging LED control
    if (bms.status.state == BMS_STATE_NORMAL || bms.status.state == BMS_STATE_CHG) {
        if (bms.status.pack_current > bms.conf.bal_idle_current && ((count / 2) % 10) == 0) {
            // not in idle: ____ ____ ____
            leds_chg_set(0);
        }
        else {
            // idle: ______________
            leds_chg_set(1);
        }
    }
    else {
        if (bms_chg_error(bms.status.error_flags)) {
            // quick flash
            leds_chg_set(count % 2);
        }
        else if (((count / 2) % 10) == 0) {
            // off without error: _    _    _
            leds_chg_set(1);
        }
        else {
            leds_chg_set(0);
        }
    }

    // Discharging LED control
    if (bms.status.state == BMS_STATE_NORMAL || bms.status.state == BMS_STATE_DIS) {
        if (bms.status.pack_current < -bms.conf.bal_idle_current && ((count / 2) % 10) == 0) {
            // not in idle: ____ ____ ____
            leds_dis_set(0);
        }
        else {
            // idle: ______________
            leds_dis_set(1);
        }
    }
    else {
        if (bms_dis_error(bms.status.error_flags)) {
            // quick flash
            leds_dis_set(count % 2);
        }
        else if (((count / 2) % 10) == 0) {
            // off without error: _    _    _
            leds_dis_set(1);
        }
        else {
            leds_dis_set(0);
        }
    }

    count++;
}

#else

void leds_chg_set(bool on)
{
    ;
}

void leds_dis_set(bool on)
{
    ;
}

#endif // UNIT_TEST
