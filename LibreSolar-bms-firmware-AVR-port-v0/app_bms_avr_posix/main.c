/*
 * Copyright (c) The Libre Solar Project Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mcu.h"
#include "bms.h"
// #include "leds.h"
#include "button.h"
#include "eeprom.h"
#include "helper.h"
#include "config.h"
#include <math.h>

Bms bms;
volatile int16_t adc_gain;   // factory-calibrated, read out from chip (uV/LSB)
volatile int8_t adc_offset; // factory-calibrated, read out from chip (mV)

volatile bool       alert_interrupt_flag;
volatile time_t     alert_interrupt_timestamp;
volatile uint32_t   time_pressed;





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
    //printf_P(PSTR("Bat degC=%.1f\n"), bms.status.bat_temp_avg);
    //printf_P(PSTR("IC degC=%.1f\n"), bms.status.ic_temp);
    printf_P(PSTR("Bat Temp degC %.1f %.1f %.1f\n"), bms.status.bat_temps[0],bms.status.bat_temps[1],bms.status.bat_temps[2]);
    
    
    
    
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

typedef enum {
    Init_MCU,
    Init_Config,
    Init_BMS_Hardware,
    Apply_BMS,
    Update_BMS,
    Shutdown_BMS,
    Jump_Bootloader,
} app_init_level;

app_init_level main_init = Init_MCU;

int main(void) {
    mcu_init();
    // set_sleep_mode(SLEEP_MODE_PWR_SAVE);
    set_sleep_mode(SLEEP_MODE_IDLE);

    while (true) {
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
                //bms_init_status(&bms);
                //bms_init_config(&bms, CONFIG_CELL_TYPE, CONFIG_BAT_CAPACITY_AH);

                // maps for settings in protection registers
                // static const uint16_t OCD_delay_setting[8] = { 8, 20, 40, 80, 160, 320, 640, 1280 }; // ms
                // static const uint16_t OCD_threshold_setting[16] = { 17, 22, 28, 33, 39, 44, 50, 56,
                //     61, 67, 72, 78, 83, 89, 94, 100 }; // mV
                //
                // static const uint16_t UV_delay_setting[4] = { 1, 4, 8, 16 }; // s
                // static const uint16_t OV_delay_setting[4] = { 1, 2, 4, 8 };  // s

                bms.status.chg_enable = true;
                bms.status.dis_enable = true;

                bms.conf.auto_balancing_enabled = true;
                bms.conf.bal_idle_delay = 1800;          // default: 30 minutes
                bms.conf.bal_idle_current = 0.1F;        // A
                bms.conf.bal_cell_voltage_diff = 0.005F; // 5 mV

                bms.conf.thermistor_beta = 3435; // typical value for Semitec 103AT-5 thermistor
                bms.conf.nominal_capacity_Ah = 12.0F;
                bms.conf.dis_oc_limit = 35.0F;
                bms.conf.chg_oc_limit = 5.0F;

                // static const uint16_t OCD_delay_setting[8] = { 8, 20, 40, 80, 160, 320, 640, 1280 }; // ms
                // static const uint16_t OCD_threshold_setting[16] = { 17, 22, 28, 33, 39, 44, 50, 56,
                bms.conf.dis_oc_delay_ms = 3200;
                bms.conf.chg_oc_delay_ms = 3200;

                bms.conf.dis_sc_limit       = 90.0F;    // SCD_threshold { 44, 67, 89, 111, 133, 155, 178, 200 }; // mV
                bms.conf.dis_sc_delay_us    = 200;      // SCD_delay { 70, 100, 200, 400 }; // us

                bms.conf.dis_ut_limit = -20.0F;
                bms.conf.dis_ot_limit = 65.0F;
                bms.conf.chg_ut_limit = 0.0F;
                bms.conf.chg_ot_limit = 50.0F;
                bms.conf.t_limit_hyst = 5.0F;

                bms.conf.shunt_res_mOhm = 1.0F;

                // static const uint16_t UV_delay_setting[4] = { 1, 4, 8, 16 }; // s
                // static const uint16_t OV_delay_setting[4] = { 1, 2, 4, 8 };  // s
                bms.conf.cell_ov_delay_ms = 2500;
                bms.conf.cell_uv_delay_ms = 30000;

                bms.conf.cell_ov_limit = 4.25F;
                bms.conf.cell_chg_voltage = 4.20F;
                bms.conf.cell_ov_reset = 4.10F;
                bms.conf.bal_cell_voltage_min = 3.80F;
                bms.conf.cell_uv_reset = 3.15F;
                bms.conf.cell_dis_voltage = 3.10F;
                bms.conf.cell_uv_limit = 3.00F;
                bms.conf.ocv = NULL;
                bms.conf.num_ocv_points = 0;

                main_init = Init_BMS_Hardware;
                printf_P(PSTR("Config init OK\n"));
                break;

            case Init_BMS_Hardware:
                if (bms_init_hardware(&bms) != 0) {
                    printf_P(PSTR("BMS hardware initialization failed\n"));
                } else {
                    main_init = Apply_BMS;

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
                pppf();
                printf_P(PSTR("Apply param OK\n"));
                break;

            case Update_BMS:

                if (elapsed_250ms()) {
                    leds_chg_set(true);
                    leds_dis_set(true);
                    bms_update(&bms);
                    bms_state_machine(&bms);
                    leds_chg_set(false);
                    leds_dis_set(false);
                }

                sleep_enable();
                sleep_cpu();

                break;

            case Shutdown_BMS:
                    bms_shutdown();
                break;

            case Jump_Bootloader:
                break;
        }

        if (elapsed_one_sec()) {
            if (main_init == Update_BMS) {
                printf_P(PSTR(" %3.3f;"), bms.status.pack_current);
                printf_P(PSTR(" %2.2f;"), bms.status.pack_voltage);
                for (uint8_t x = 0; x < BOARD_NUM_CELLS_MAX; ++x) {
                    printf_P(PSTR(" %1.3f;"), bms.status.cell_voltages[x]);
                }
                printf_P(PSTR(" %3.2f;"), bms.status.bat_temps[0]);
                printf_P(PSTR(" %3.2f;"), bms.status.bat_temps[1]);
                printf_P(PSTR(" %3.2f;\n"), bms.status.bat_temps[2]);
            }
        }

    }
}
