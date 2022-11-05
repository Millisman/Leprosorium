#include "mcu/mcu.h"
#include <stdio.h>
#include "app_card/card_reader.h"
#include "app_charger/charger.h"
#include "app_gsm/gsm.h"
#include "app_gui/gui.h"
#include "app_radio/radio.h"
#include "app_store/eeprom_store.h"
#include "app_meter/meter.h"
#include "console/console.h"
#include <util/delay.h>
#include "mcu/LiquidCrystal4b.h"
#include "app_ocpp/ocpp_man.h"
static uint8_t ccc = 0;
static uint8_t xxx = 0;
int main() {
    mcu_init_all();
    Console_init();
    app_Store_init();
    app_GUI_init();
//     /*_delay_ms*/(5);
    app_Radio_init();
    app_Meter_init();
//     _delay_ms(5);
    app_CardReader_init();
    _delay_ms(5);
    app_Gsm_init();
    OCPP_init();
    OCPP_begin();
    app_Charger_init();
    
    fprintf_P(stdout, PSTR("Start OK\n"));
    while (1) {

        switch (mcu_get_elapses()) {
            case ELAPSED_100ms:
                
                
                
                LiquidCrystal_setCursor(ccc++, xxx);
                if (ccc >= 20) { ccc = 0; 
                    if (xxx == 0) {
                        xxx = 3;
                    } else {
                        xxx = 0;
                    }
                }
                fprintf_P(stderr, PSTR("\xFF"));
                app_Charger_update_100ms();
                app_Gsm_update_100ms();
                app_GUI_update_100ms();
                app_Radio_update_100ms();
                Console_update_100ms();
                app_CardReader_update_100ms();
                app_Meter_update_100ms();
                OCPP_tick_100ms();
                break;
            case ELAPSED_1S:
                fprintf_P(stderr, PSTR(" HELLO "));
                app_Meter_update_1s();
                app_Charger_update_1s();
                app_Gsm_update_1s();
                app_GUI_update_1s();
                app_Store_update_1s();
                app_CardReader_update_1s();
                app_Radio_update_1s();
                Console_update_1S();
                OCPP_tick_1sec();
                break;
            default:
            {
                app_Meter_update();
                app_Charger_update();
                app_CardReader_update();
                app_Gsm_update();
                app_GUI_update();
                app_Radio_update();
                Console_update();
            }

        }

       wdt_reset();
    }

}
