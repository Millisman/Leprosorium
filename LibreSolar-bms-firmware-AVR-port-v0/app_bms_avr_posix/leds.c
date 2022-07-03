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
