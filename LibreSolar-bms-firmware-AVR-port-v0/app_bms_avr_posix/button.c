/*
 * Copyright (c) The Libre Solar Project Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mcu.h"
#include "config.h"
#include "button.h"
#include "helper.h"

extern uint32_t time_pressed;

bool button_pressed_for_3s()
{
#if defined (__AVR__) || defined (__AVR_ARCH__)
    return ACTIVE(PIN_BTN) && (millis2() - time_pressed) > 3000;
#else
    // TODO receive internal signal
    return false;
#endif
}
