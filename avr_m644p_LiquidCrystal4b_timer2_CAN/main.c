/*
    MIT License

    Copyright (c) 2022 Sergey Kostyanoy

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include "mcu/mcu.h"
#include <stdio.h>
#include "console/console.h"
#include "app_canbus/can_bus.h"

int main() {
    mcu_init_all();
    CAN_init();

    while (1) {

        switch (mcu_get_elapses()) {
            case ELAPSED_100ms:
                fprintf_P(stderr, PSTR(" TEST "));
                CAN_update_100ms();
                break;
            case ELAPSED_1S:
                fprintf_P(stderr, PSTR(" HELLO "));
                //printf_P(PSTR("..1sec\n"));
                CAN_update_1S();
                break;
            default:
            {
                Console_update();
                CAN_update();
            }

        }

       wdt_reset();
    }

}
