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
#include <math.h>

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

float fx(float f) { return isnan(f) ? 0 : f; }

void printFloat(const char *label, double number, uint8_t digits) {
    
    printf("%s=", label);
    
    if (isnan(number)) { printf_P(PSTR("nan")); return; }
    if (isinf(number)) { printf_P(PSTR("inf")); return; }
    if (number > 4294967040.0) { printf_P(PSTR("ovf")); return; } // constant determined empirically
    if (number <-4294967040.0) { printf_P(PSTR("ovf")); return; } // constant determined empirically
    
    // Handle negative numbers
    if (number < 0.0) { number = -number; }
    
    // Round correctly so that print(1.999, 2) prints as "2.00"
    double rounding = 0.5;
    for (uint8_t i=0; i<digits; ++i) {
        rounding /= 10.0;
    }
    
    number += rounding;
    
    // Extract the integer part of the number and print it
    unsigned long int_part = (unsigned long)number;
    double remainder = number - (double)int_part;
    printf("%lu", int_part);
//     n += print(int_part);
    
    // Print the decimal point, but only if there are digits beyond
    if (digits > 0) { printf("."); }
    
    // Extract digits from the remainder one at a time
    while (digits-- > 0) {
        remainder *= 10.0;
        unsigned int toPrint = (unsigned int)(remainder);
        printf("%u", toPrint);
        remainder -= toPrint; 
    }
    
    printf("\n");
}



// %6lf
void ppp() {
    printFloat("\n\nBatNom Ah", bms.conf.nominal_capacity_Ah, 1);
    
    // current limits

    printFloat("PcbDisSC A", bms.conf.dis_sc_limit, 1);
    printf_P(PSTR("PcbDisSC_us=%u\n"), bms.conf.dis_sc_delay_us);
    printFloat("BatDisLim A", bms.conf.dis_oc_limit, 1);
    printf_P(PSTR("BatDisLimDelay ms=%u\n"), bms.conf.dis_oc_delay_ms);
    printFloat("BatChgLim A", bms.conf.chg_oc_limit, 1);
    printf_P(PSTR("BatChgLimDelay_ms=%u\n"), bms.conf.chg_oc_delay_ms);

// temperature limits
printFloat("DisUpLim degC", bms.conf.dis_ot_limit, 1);
printFloat("DisLowLim degC", bms.conf.dis_ut_limit, 1);
printFloat("ChgUpLim degC", bms.conf.chg_ot_limit, 1);
printFloat("ChgLowLim degC", bms.conf.chg_ut_limit, 1);
printFloat("TempLimHyst degC", bms.conf.t_limit_hyst, 1);

// voltage limits
printFloat("CellUpLim V", bms.conf.cell_ov_limit, 3);
printFloat("CellUpLimResetV", bms.conf.cell_ov_reset, 3);
printf_P(PSTR("CellUpLimDelay_ms=%u\n"), bms.conf.cell_ov_delay_ms);
printFloat("CellLowLim V", bms.conf.cell_uv_limit, 3);
printFloat("CellLowLimReset V", bms.conf.cell_uv_reset, 3);
printf_P(PSTR("CellLowLimDelay_ms=%u\n"), bms.conf.cell_uv_delay_ms);

// balancing
printf_P(PSTR("AutoBalEn=%u\n"), bms.conf.auto_balancing_enabled);
printFloat("BalCellDiff V", bms.conf.bal_cell_voltage_diff, 4);
printFloat("BalCellLowLim V", bms.conf.bal_cell_voltage_min, 4);
printf_P(PSTR("BalIdleDelay_s=%u\n"), bms.conf.bal_idle_delay);
printFloat("BalIdleTh A", bms.conf.bal_idle_current, 4);

// INPUT DATA /////////////////////////////////////////////////////////////
printf_P(PSTR("ChgEn %u\n"), bms.status.chg_enable);
printf_P(PSTR("DisEn %u\n"), bms.status.dis_enable);
// OUTPUT DATA ////////////////////////////////////////////////////////////

//TS_GROUP(ID_MEAS, "Meas", TS_NO_CALLBACK, ID_ROOT),
printFloat("Bat V", bms.status.pack_voltage, 4);
printFloat("Bat A", bms.status.pack_current, 4);
printFloat("Bat degC", bms.status.bat_temp_avg, 4);
printFloat("IC degC", bms.status.ic_temp, 4);
#ifdef CONFIG_ISL94202   // currently only implemented in ISL94202-based boards (using TS2)
printf_P(PSTR("MOSFET_degC="), &bms.status.mosfet_temp, 4);
#endif
printFloat("SOC pct", bms.status.soc, 2);
printf_P(PSTR("ErrorFlags=%u\n"), bms.status.error_flags);
printf_P(PSTR("BmsState=%u\n"), bms.status.state);

// printf_P(PSTR("Cells_V="));
for (uint8_t x = 0; x < BOARD_NUM_CELLS_MAX; ++x) {
    printf_P(PSTR(" Cell %u="), x+1);
    
    printFloat("", bms.status.cell_voltages[x], 3);
}


// printf_P(PSTR("rCells_V"), &cell_voltages_arr);
printFloat("CellAvg V", bms.status.cell_voltage_avg, 4);
printFloat("CellMin V", bms.status.cell_voltage_min, 4);
printFloat("CellMax V", bms.status.cell_voltage_max, 4);
printf_P(PSTR("BalancingStatus=%u\n"), bms.status.balancing_status);
}


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
        
        if (time_now - exec_1s > 2000) {
            exec_1s = time_now;
            if (main_init == Update_BMS) {
                //bms_print_registers();
                ppp();
            }
        }
        
        // TODO goto sleep
    }
}
