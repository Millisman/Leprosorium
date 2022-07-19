#include "mcu/mcu.h"
#include <stdio.h>
#include "console/console.h"

int main() {
    mcu_init_all();
    fprintf_P(stdout, PSTR("Init OK\n"));

    while (1) {

        switch (mcu_get_elapses()) {
            case ELAPSED_100ms:
                fprintf_P(stderr, PSTR(" TEST "));

                break;
            case ELAPSED_1S:
                fprintf_P(stderr, PSTR(" HELLO "));
                printf_P(PSTR("..1sec\n"));

//                 if (lcd_Block > 0) {
//                     --lcd_Block;
//                     if (lcd_Block == 0) { app::LiquidCrystal::clear(); }
//                 }
                break;
            default:
            {
                Console_update();
            }

        }

       wdt_reset();
    }

}
