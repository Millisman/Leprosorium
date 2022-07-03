/*
 * Copyright (c) The Libre Solar Project Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mcu.h"
#include "bms.h"
#include "leds.h"
#include "button.h"
#include "eeprom.h"
#include "helper.h"
#include "config.h"

Bms bms;

volatile bool       alert_interrupt_flag;
volatile time_t     alert_interrupt_timestamp;
volatile uint32_t   time_pressed;


typedef enum {
    Init_MCU,
    Init_Config,
    Init_BMS_Hardware,
    Apply_BMS,
    Update_BMS,
    Shutdown_BMS,
    Jump_Bootloader,
} app_init_level;


app_init_level main_init;
    

int main(void) {
    mcu_init();
    uint32_t time_now;
    uint32_t exec_100ms = 0;
    uint32_t exec_1s    = 0;
    main_init = Init_MCU;
    
    while (true) {
        time_now = millis2();
        
        if(time_now - exec_100ms >= 250) {
            exec_100ms = time_now;
            
            switch (main_init) {
                case Init_MCU:
                    printf_P(PSTR("Hardware: Libre Solar " CONFIG_PCB_TYPE " (" CONFIG_PCB_VENDOR ")\n"));
                    printf_P(PSTR("Firmware: " CONFIG_FIRMWARE_VERSION_ID "\n"));
                    leds_init();
                    i2c_master_init();
                    main_init = Init_Config;
                    printf_P(PSTR("MCU init OK\n"));
                    break;
                    
                case Init_Config:
                    bms_init_status(&bms);
                    bms_init_config(&bms, CONFIG_CELL_TYPE, CONFIG_BAT_CAPACITY_AH);
                    main_init = Init_BMS_Hardware;
                    printf_P(PSTR("Config init OK\n"));
                    break;
                    
                case Init_BMS_Hardware:
                    if (bms_init_hardware(&bms) != 0) {
                        printf_P(PSTR("BMS hardware initialization failed\n"));
                    } else {
                        main_init = Apply_BMS;
                        printf_P(PSTR("BMS hardware init OK\n"));
                    }
                    break;
                    
                case Apply_BMS:
                    bms_apply_cell_ovp(&bms);
                    bms_apply_cell_uvp(&bms);
                    
                    bms_apply_dis_scp(&bms);
                    bms_apply_dis_ocp(&bms);
                    bms_apply_chg_ocp(&bms);
                    
                    bms_apply_temp_limits(&bms);
                    
                    bms_update(&bms);
                    bms_soc_reset(&bms, -1);
                    main_init = Update_BMS;
                    printf_P(PSTR("Apply param OK\n"));
                    break;
                    
                case Update_BMS:
                    bms_update(&bms);
                    bms_state_machine(&bms);
                    leds_update();
                    break;
                    
                case Shutdown_BMS:
                     bms_shutdown();
                    break;
                    
                case Jump_Bootloader:
                    break;
            }
        }
        
        if (time_now - exec_1s > 1000) {
            exec_1s = time_now;
            if (main_init == Update_BMS) { bms_print_registers(); }
        }
        
        // TODO goto sleep
    }
}
