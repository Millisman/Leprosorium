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
volatile int adc_gain;   // factory-calibrated, read out from chip (uV/LSB)
volatile int adc_offset; // factory-calibrated, read out from chip (mV)

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


// BatNom Ah=100.00
// PcbDisSC A=200.00
// PcbDisSC_us=200
// BatDisLim A=100.00
// BatDisLimDelay ms=320
// BatChgLim A=100.00
// BatChgLimDelay_ms=320
// DisUpLim degC=45.00
// DisLowLim degC=-20.00
// ChgUpLim degC=45.00
// ChgLowLim degC=0.00
// TempLimHyst degC=5.00
// CellUpLim V=4246.00
// CellUpLimResetV=4.05
// CellUpLimDelay_ms=8
// CellLowLim V=3000.00
// CellLowLimReset V=3.50
// CellLowLimDelay_ms=16
// AutoBalEn=1
// BalCellDiff V=0.01
// BalCellLowLim V=3.80
// BalIdleDelay_s=1800
// BalIdleTh A=0.10
// ChgEn 1
// DisEn 1
// Bat V=47.43
// Bat A=1.55
// Bat degC=31.2
// IC degC=0.0
// SOC pct=44.28
// ErrorFlags=0
// BmsState=3
// ConnectedCells=13
// Cell 1=3.645 Cell 2=3.653 Cell 3=3.653 Cell 4=3.649 Cell 5=3.650 Cell 6=3.646 Cell 7=3.649 Cell 8=3.646
// Cell 9=0.044 Cell 10=3.646 Cell 11=3.648 Cell 12=3.649 Cell 13=3.650 Cell 14=0.046 Cell 15=3.645
// CellAvg V=3.648
// CellMin V=3.645
// CellMax V=3.653
// BalancingStatus=0
void pppf() {
    
    printf_P(PSTR("\n\nBatNom Ah=%.2f\n"), bms.conf.nominal_capacity_Ah);
    
    printf_P(PSTR("PcbDisSC A=%.2f\n"), bms.conf.dis_sc_limit);
    printf_P(PSTR("PcbDisSC_us=%u\n"), bms.conf.dis_sc_delay_us);
    printf_P(PSTR("BatDisLim A=%.2f\n"), bms.conf.dis_oc_limit);
    printf_P(PSTR("BatDisLimDelay ms=%u\n"), bms.conf.dis_oc_delay_ms);
    printf_P(PSTR("BatChgLim A=%.2f\n"), bms.conf.chg_oc_limit);
    printf_P(PSTR("BatChgLimDelay_ms=%u\n"), bms.conf.chg_oc_delay_ms);
    
    // temperature limits
    printf_P(PSTR("DisUpLim degC=%.2f\n"), bms.conf.dis_ot_limit);
    printf_P(PSTR("DisLowLim degC=%.2f\n"), bms.conf.dis_ut_limit);
    printf_P(PSTR("ChgUpLim degC=%.2f\n"), bms.conf.chg_ot_limit);
    printf_P(PSTR("ChgLowLim degC=%.2f\n"), bms.conf.chg_ut_limit);
    printf_P(PSTR("TempLimHyst degC=%.2f\n"), bms.conf.t_limit_hyst);
    
    // voltage limits
    printf_P(PSTR("CellUpLim V=%.2f\n"), bms.conf.cell_ov_limit);
    printf_P(PSTR("CellUpLimResetV=%.2f\n"), bms.conf.cell_ov_reset);
    printf_P(PSTR("CellUpLimDelay_ms=%u\n"), bms.conf.cell_ov_delay_ms);
    printf_P(PSTR("CellLowLim V=%.2f\n"), bms.conf.cell_uv_limit);
    printf_P(PSTR("CellLowLimReset V=%.2f\n"), bms.conf.cell_uv_reset);
    printf_P(PSTR("CellLowLimDelay_ms=%u\n"), bms.conf.cell_uv_delay_ms);
    
    // balancing
    printf_P(PSTR("AutoBalEn=%u\n"), bms.conf.auto_balancing_enabled);
    printf_P(PSTR("BalCellDiff V=%.2f\n"), bms.conf.bal_cell_voltage_diff);
    printf_P(PSTR("BalCellLowLim V=%.2f\n"), bms.conf.bal_cell_voltage_min);
    printf_P(PSTR("BalIdleDelay_s=%u\n"), bms.conf.bal_idle_delay);
    printf_P(PSTR("BalIdleTh A=%.2f\n"), bms.conf.bal_idle_current);
    
    // INPUT DATA /////////////////////////////////////////////////////////////
    printf_P(PSTR("ChgEn %u\n"), bms.status.chg_enable);
    printf_P(PSTR("DisEn %u\n"), bms.status.dis_enable);
    // OUTPUT DATA ////////////////////////////////////////////////////////////
    
    //TS_GROUP(ID_MEAS, "Meas", TS_NO_CALLBACK, ID_ROOT),
    printf_P(PSTR("Bat V=%.2f\n"), bms.status.pack_voltage);
    printf_P(PSTR("Bat A=%.2f\n"), bms.status.pack_current);
    printf_P(PSTR("Bat degC=%.1f\n"), bms.status.bat_temp_avg);
    printf_P(PSTR("IC degC=%.1f\n"), bms.status.ic_temp);
#ifdef CONFIG_ISL94202   // currently only implemented in ISL94202-based boards (using TS2)
printf_P(PSTR("MOSFET_degC="), &bms.status.mosfet_temp, 4);
#endif
    printf_P(PSTR("SOC pct=%.2f\n"), bms.status.soc);
    printf_P(PSTR("ErrorFlags=%u\n"), bms.status.error_flags);
    printf_P(PSTR("BmsState=%u\n"), bms.status.state);
    
    printf_P(PSTR("ConnectedCells=%u\n"), bms.status.connected_cells);
    
    for (uint8_t x = 0; x < BOARD_NUM_CELLS_MAX; ++x) {
        printf_P(PSTR(" Cell %u=%.3f"), x+1, bms.status.cell_voltages[x]);
        
//         printFloat("=%.2f\n", bms.status.cell_voltages[x], 3);
    }
    
    
    // printf_P(PSTR("rCells_V"), &cell_voltages_arr);
    printf_P(PSTR("\nCellAvg V=%.3f\n"), bms.status.cell_voltage_avg);
    printf_P(PSTR("CellMin V=%.3f\n"), bms.status.cell_voltage_min);
    printf_P(PSTR("CellMax V=%.3f\n"), bms.status.cell_voltage_max);
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
                        
//                         Bms bms;
//                         volatile int adc_gain;   // factory-calibrated, read out from chip (uV/LSB)
//                         volatile int adc_offset; // factory-calibrated, read out from chip (mV)
                        
                        printf_P(PSTR("adc_gain=%d uV/LSB, adc_offset=%d mV\n"), adc_gain, adc_offset);
                        
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
                pppf();
            }
        }
        
        // TODO goto sleep
    }
}
