/*
 * Copyright (c) The Libre Solar Project Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mcu.h"
#include "bms.h"
#include "button.h"
#include "eeprom.h"
#include "helper.h"
#include "leds.h"

Bms bms;
volatile bool alert_interrupt_flag;
volatile time_t alert_interrupt_timestamp;
volatile uint32_t time_pressed;

int main(void)
{
    mcu_init();
    leds_init();
    twi_master_init();
    
    printf_P(PSTR("Hardware: Libre Solar %s (%s)\n"), "PCB_TYPE", "PCB_VENDOR");
    printf_P(PSTR("Firmware: %s\n"), FIRMWARE_VERSION_ID);

    bms_init_status(&bms);
    bms_init_config(&bms, CONFIG_CELL_TYPE, CONFIG_BAT_CAPACITY_AH);

    while (bms_init_hardware(&bms) != 0) {
        printf_P(PSTR("BMS hardware initialization failed\n"));
    }
    
    bms_apply_cell_ovp(&bms);
    bms_apply_cell_uvp(&bms);

    bms_apply_dis_scp(&bms);
    bms_apply_dis_ocp(&bms);
    bms_apply_chg_ocp(&bms);

    bms_apply_temp_limits(&bms);

    bms_update(&bms);
    bms_soc_reset(&bms, -1);

    //init_Int1(); // + button
    
    uint32_t exec_100ms = 0;
    uint32_t exec_1s    = 0;
    uint32_t exec_6h    = 0;
    
    while (true) {
        
        uint32_t time_now = millis2();
        
        if(time_now - exec_100ms > 100) {
            exec_100ms = time_now;
            
            bms_update(&bms);
            bms_state_machine(&bms);
            leds_update();
            
            // if (button_pressed_for_3s()) {
            //     printf("Button pressed for 3s: shutdown...\n");
            //     bms_shutdown();
            //     while (true) { }
            // }
            
            if (time_now - exec_1s > 1000) {
                exec_1s = time_now;
//                  printf("1s\n");
                bms_print_registers();
            }
            
            if (time_now - exec_6h > EEPROM_UPDATE_INTERVAL * 1000) {
                exec_6h = time_now;
                eeprom_store_data();
            }
        }
    }
}

// #endif /* UNIT_TEST */
