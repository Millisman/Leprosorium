/**
*  @file config.h
*
*  Project-wide hardware declarations.
*
*  Copyright © 2022 Sergey A. Kostyanoy aka Millisman
**/

#pragma once

#define PIN_BTN              D, 3, H // PD3 - INT1 BUTTON
#define PIN_LED_CHG          B, 5, H // PB5 - LED OUT BLUE
#define PIN_LED_DIS          B, 5, H // PB5 - LED OUT BLUE

#define FIRMWARE_VERSION_ID "VERSION_ID"
// CELL_TYPE_CUSTOM,    ///< Custom settings
// CELL_TYPE_LFP,       ///< LiFePO4 Li-ion cells (3.3 V nominal)
// CELL_TYPE_NMC,       ///< NMC/Graphite Li-ion cells (3.7 V nominal)
// CELL_TYPE_NMC_HV,    ///< NMC/Graphite High Voltage Li-ion cells (3.7 V nominal, 4.35 V max)
// CELL_TYPE_LTO        ///< NMC/Titanate (2.4 V nominal)

#define CONFIG_CELL_TYPE CELL_TYPE_NMC
#define CONFIG_BAT_CAPACITY_AH 100.0F
#define BOARD_NUM_CELLS_MAX       15 //DT_PROP(DT_PATH(pcb), num_cells_max)
#define BOARD_NUM_THERMISTORS_MAX 3 //DT_PROP(DT_PATH(pcb), num_thermistors_max)
#define BOARD_MAX_CURRENT          100 //DT_PROP(DT_PATH(pcb), current_max)
#define BOARD_SHUNT_RESISTOR       1.0 //(DT_PROP(DT_PATH(pcb), shunt_res) / 1000.0)
#define EEPROM_UPDATE_INTERVAL (6 * 60 * 60) // update every 6 hours

#define CONFIG_PCB_TYPE                 "not365 Type"
#define CONFIG_PCB_VENDOR               "Millisman"
#define CONFIG_FIRMWARE_VERSION_ID      "0.3a"
