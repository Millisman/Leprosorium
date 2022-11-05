/* s3emul - "secu3 emulator"
 *   Copyright (C) 2016 andreika. Ukraine, Kiev
 *
 *   The following code is a derivative work of the code from the SECU-3 project,
 *   which is licensed GPLv3. This code therefore is also licensed under the terms
 *   of the GNU Public License, verison 3.
 *
 *   PORTIONS OF THIS CODE TAKEN FROM:
 *
 *   SECU-3  - An open source, free engine control unit
 *   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   contacts:
 *              http://secu-3.org
 *              email: shabelnikov@secu-3.org
 */

#include <stdint.h>

#pragma once

#pragma pack(push, 1)

#define F_WRK_POINTS_F                  16          //!< number of points on RPM axis - work map
#define F_WRK_POINTS_L                  16          //!< number of points on Pressure axis - work map
#define F_TMP_POINTS                    16          //!< number of points in temperature map
#define F_STR_POINTS                    16          //!< number of points in start map
#define F_IDL_POINTS                    16          //!< number of points in idle map
#define F_WRK_TOTAL (F_WRK_POINTS_L*F_WRK_POINTS_F) //!< total size of work map

#define F_NAME_SIZE                     16          //!< number of symbols in names of tables' sets

#define KC_ATTENUATOR_LOOKUP_TABLE_SIZE 128         //!< number of points in attenuator's lookup table
#define FW_SIGNATURE_INFO_SIZE          48          //!< number of bytes reserved for firmware's signature information
#define COIL_ON_TIME_LOOKUP_TABLE_SIZE  32          //!< number of points in lookup table used for dwell control
#define THERMISTOR_LOOKUP_TABLE_SIZE    16          //!< Size of lookup table for coolant temperature sensor
#define CHOKE_CLOSING_LOOKUP_TABLE_SIZE 16          //!< Size of lookup table defining choke closing versus coolant temperature
#define ATS_CORR_LOOKUP_TABLE_SIZE      16          //!< Air temperature sensor advance angle correction lookup table
#define RPM_GRID_SIZE                   16          //!< Number of points on the RPM axis in advance angle lookup tables
#define IBTN_KEYS_NUM                   2           //!< Number of iButton keys
#define IBTN_KEY_SIZE                   6           //!< Size of iButton key (except CRC8 and family code)

#define INJ_VE_POINTS_L                 16          //!< number of points on MAP axis in VE lookup table
#define INJ_VE_POINTS_F                 16          //!< number of points on RPM axis in VE lookup table
#define INJ_DT_LOOKUP_TABLE_SIZE        32          //!< number of points in the injector dead-time lookup table
#define INJ_CRANKING_LOOKUP_TABLE_SIZE  16          //!< number of points in the cranking lookup table
#define INJ_WARMUP_LOOKUP_TABLE_SIZE    16          //!< number of points in the warmup enrichment lookup table
#define INJ_IAC_POS_TABLE_SIZE          16          //!< number of points in the IAC/PWM position lookup table
#define INJ_AE_TPS_LOOKUP_TABLE_SIZE    8           //!< number of points in AE TPS (d%/dt) lookup table
#define INJ_AE_RPM_LOOKUP_TABLE_SIZE    4           //!< number of points in AE RPM lookup table size
#define INJ_AFTSTR_LOOKUP_TABLE_SIZE    16          //!< afterstart enrichment lookup table

//Injection configuration constants
#define INJCFG_THROTTLEBODY             0           //!< Throttle-body or central injection
#define INJCFG_SIMULTANEOUS             1           //!< Simultaneous port injection
#define INJCFG_2BANK_ALTERN             2           //!< 2 banks alternating injection
#define INJCFG_SEMISEQUENTIAL           3           //!< Semi-sequential injection
#define INJCFG_FULLSEQUENTIAL           4           //!< Full-sequential injection

#define UNI_OUTPUT_NUMBER               3           //!< number of universal programmable outputs

#define GASDOSE_POS_RPM_SIZE            16          //!< RPM axis size
#define GASDOSE_POS_TPS_SIZE            16          //!< TPS axis size

#define TABLES_NUMBER_PGM               4
#define TABLES_NUMBER          (TABLES_NUMBER_PGM+1)

#define OPCODE_EEPROM_PARAM_SAVE     1    //!< save EEPROM parameters
#define OPCODE_CE_SAVE_ERRORS        2    //!< save CE errors
#define OPCODE_READ_FW_SIG_INFO      3    //!< read signature information about firmware
#define OPCODE_LOAD_TABLSET          4    //!< new set of tables must be loaded or notify that it has been loaded
#define OPCODE_SAVE_TABLSET          5    //!< save table set for selected fuel from RAM to EEPROM or notify that it has been saved
#define OPCODE_DIAGNOST_ENTER        6    //!< enter diagnostic mode
#define OPCODE_DIAGNOST_LEAVE        7    //!< leave diagnostic mode
#define OPCODE_RESET_EEPROM       0xCF    //!< reset EEPROM, second byte must be 0xAA

// little-endian data store format (Intel)
#define _AB(src,rel) *(((unsigned char*)&(src)+(rel)))                 /**< Access Nth byte in variable */

#define IOP_MAPSEL0      47
#define IOCFG_CHECK(io_id) ((&fw.cddata.iorem.s_stub) != (&fw.cddata.iorem.i_plugs[io_id]))

typedef struct uni_output_t
{
    uint8_t flags;                          //!< MS Nibble - logic function, LS Nibble - flags (inversion)
    uint8_t condition1;                     //!< code of condition 1
    uint8_t condition2;                     //!< code of condition 2
    uint16_t on_thrd_1;                     //!< ON threshold (if value > on_thrd_1)
    uint16_t off_thrd_1;                    //!< OFF threshold (if value < off_thrd_1)
    uint16_t on_thrd_2;                     //!< same as on_thrd_1
    uint16_t off_thrd_2;                    //!< same as off_thrd_1
}uni_output_t;

typedef struct params_t
{
    // Cranking
    uint16_t starter_off;                  //!< RPM when starter will be turned off
    uint16_t smap_abandon;                 //!< RPM when switching from start map(min-1)

    // MAP sensor
    uint16_t map_lower_pressure;           //!< lower value of MAP at the axis of table(work map) (kPa)
    int16_t  map_upper_pressure;           //!< upper value of MAP at the axis of table(work map) (kPa)
    int16_t  map_curve_offset;             //!< offset of curve in volts, can be negative
    int16_t  map_curve_gradient;           //!< gradient of curve in kPa/V, can be negative (inverse characteristic curve)

    // TPS sensor/limit switch
    uint8_t  carb_invers;                  //!< flag of inversion of carburetor's limit switch
    int16_t  tps_curve_offset;             //!< offset of curve in volts
    int16_t  tps_curve_gradient;           //!< gradient of curve in Percentage/V
    uint8_t  tps_threshold;                //!< TPS threshold used to switch work and idle modes (if 0 then input is treated as digital and simple switch is used)

    // Idle cut-off valve and power valve
    uint16_t ie_lot;                       //!< lower threshold for idle cut off valve(min-1) for gasoline
    uint16_t ie_hit;                       //!< upper threshold for idle cut off valve(min-1) for gasoline
    uint16_t ie_lot_g;                     //!< lower threshold for idle cut off valve(min-1) for gas
    uint16_t ie_hit_g;                     //!< upper threshold for idle cut off valve(min-1) for gas
    int16_t  fe_on_threshold;              //!< switch on threshold of power valve (FE)
    uint8_t  shutoff_delay;                //!< idle cut off valve's turn off delay
    uint16_t fuelcut_map_thrd;             //!< fuel cut off MAP threshold
    int16_t fuelcut_cts_thrd;              //!< fuel cut off CTS threshold

    // Advance angle control
    int16_t  angle_dec_speed;              //!< limitation of alternation speed of advance angle (when decreasing)
    int16_t  angle_inc_speed;              //!< limitation of alternation speed of advance angle (when increasing)
    int16_t  max_angle;                    //!< system's maximum advance angle limit
    int16_t  min_angle;                    //!< system's minimum advance angle limit
    int16_t  angle_corr;                   //!< octane correction of advance angle
    uint8_t  zero_adv_ang;                 //!< Zero advance angle flag

    uint8_t  fn_gasoline;                  //!< index of set of characteristics used for gasoline
    uint8_t  fn_gas;                       //!< index of set of characteristics used for gas

    // Idling regulator (via advance angle)
    uint8_t  idl_flags;                    //!< Idling regulator flags (see IRF_x constants for more information)
    uint16_t idling_rpm;                   //!< selected idling RPM regulated by using advance angle
    int16_t  ifac1;                        //!< Idling regulator's factor for positive error
    int16_t  ifac2;                        //!< Idling regulator's factor for negative error
    int16_t  MINEFR;                       //!< dead band of idling regulator (min-1)
    int16_t  idlreg_min_angle;             //!< minimum advance angle correction which can be produced by idling regulator
    int16_t  idlreg_max_angle;             //!< maximum advance angle correction which can be produced by idling regulator
    int16_t  idlreg_turn_on_temp;          //!< Idling regulator turn on temperature

    // Temperature and cooling fan control
    uint8_t  tmp_use;                      //!< flag of using coolant sensor
    int16_t  vent_on;                      //!< cooling fan's turn on temperature
    int16_t  vent_off;                     //!< cooling fan's turn off temperature
    uint8_t  vent_pwm;                     //!< flag - control cooling fan by using PWM
    uint8_t  cts_use_map;                  //!< Flag which indicates using of lookup table for coolant temperature sensor
    uint16_t vent_pwmfrq;                  //!< PWM frequency (value = 1/f * 524288), 10....5000Hz

    // ADC corrections/compensations
    int16_t  map_adc_factor;               //!< ADC error compensation factor for MAP
    int32_t  map_adc_correction;           //!< ADC error compensation correction for MAP
    int16_t  ubat_adc_factor;              //!< ADC error compensation factor for board voltage
    int32_t  ubat_adc_correction;          //!< ADC error compensation correction for board voltage
    int16_t  temp_adc_factor;              //!< ADC error compensation factor for CLT
    int32_t  temp_adc_correction;          //!< ADC error compensation correction for CLT
    int16_t  tps_adc_factor;               //!< ADC error compensation factor for TPS
    int32_t  tps_adc_correction;           //!< ADC error compensation correction for TPS
    int16_t  ai1_adc_factor;               //!< ADC error compensation factor for ADD_IO1 input
    int32_t  ai1_adc_correction;           //!< ADC error compensation correction for ADD_IO1 input
    int16_t  ai2_adc_factor;               //!< ADC error compensation factor for ADD_IO2 input
    int32_t  ai2_adc_correction;           //!< ADC error compensation correction for ADD_IO2 input

    // Synchronization
    uint8_t  ckps_edge_type;               //!< Edge type for interrupt from CKP sensor (rising or falling edge). Depends on polarity of sensor
    uint8_t  ckps_cogs_btdc;               //!< Teeth before TDC
    uint8_t  ckps_ignit_cogs;              //!< Duration of ignition driver's pulse countable in teeth of wheel
    uint8_t  ckps_engine_cyl;              //!< number of engine's cylinders
    uint8_t  ckps_cogs_num;                //!< number of crank wheel's teeth
    uint8_t  ckps_miss_num;                //!< number of missing crank wheel's teeth
    uint8_t  ref_s_edge_type;              //!< Edge type of REF_S input
    uint8_t  hall_flags;                   //!< Hall sensor related flags
    int16_t  hall_wnd_width;               //!< Hall sensor's shutter window width in degrees of crankshaft (advance value of distributor)

    // Ignition outputs control
    uint8_t  ign_cutoff;                   //!< Cutoff ignition when RPM reaches specified threshold
    uint16_t ign_cutoff_thrd;              //!< Cutoff threshold (RPM)
    uint8_t  merge_ign_outs;               //!< Merge ignition signals to single output flag

    // Hall emulation output
    int8_t   hop_start_cogs;               //!< Hall output: start of pulse in teeth relatively to TDC
    uint8_t  hop_durat_cogs;               //!< Hall output: duration of pulse in teeth

    // Communication
    uint16_t uart_divisor;                 //!< divider which corresponds to selected baud rate
    uint8_t  uart_period_t_ms;             //!< transmition period of data packets which SECU-3 sends, one discrete = 10ms

    // Knock control
    uint8_t  knock_use_knock_channel;      //!< flag of using knock channel
    uint8_t  knock_bpf_frequency;          //!< Band pass filter frequency
    int16_t  knock_k_wnd_begin_angle;      //!< Opening angle of knock phase window
    int16_t  knock_k_wnd_end_angle;        //!< Closing angle of knock phase window
    uint8_t  knock_int_time_const;         //!< Integration time constant
    int16_t  knock_retard_step;            //!< Displacement(retard) step of angle
    int16_t  knock_advance_step;           //!< Recovery step of angle
    int16_t  knock_max_retard;             //!< Maximum displacement of angle
    uint16_t knock_threshold;              //!< detonation threshold - voltage
    uint8_t  knock_recovery_delay;         //!< Recovery delay of angle countable in engine's cycles

    // Choke control
    uint16_t sm_steps;                     //!< Number of steps of choke stepper motor
    uint16_t choke_rpm[2];                 //!< Values of RPM needed for RPM-based control of choke position
    uint8_t  choke_startup_corr;           //!< Startup correction value for choke (0...200)
    uint16_t choke_rpm_if;                 //!< Integral factor for RPM-based control of choke position (factor * 1024)
    uint16_t choke_corr_time;              //!< Time for startup correction will be applied
    int16_t  choke_corr_temp;              //!< Temperature threshold for startup correction

    // Bluetooth and security
    uint8_t  bt_flags;                     //!< Bluetooth and security related flags
    uint8_t  ibtn_keys[IBTN_KEYS_NUM][IBTN_KEY_SIZE]; //!< iButton keys for immobilizer

    uni_output_t uni_output[UNI_OUTPUT_NUMBER]; //!< parameters for versatile outputs
    uint8_t uniout_12lf;                   //!< logic function between 1st and 2nd outputs

    // Fuel injection
    uint8_t  inj_flags;                    //!< Fuel injection related flags
    uint8_t  inj_config;                   //!< Configuration of injection (7-4 bits: inj. config., 3-0 bits: num of squitrs), inj.config: INJCFG_x constants
    uint16_t inj_flow_rate;                //!< Injector flow rate (cc/min) * 64
    uint16_t inj_cyl_disp;                 //!< The displacement of one cylinder in liters * 16384
    uint32_t inj_sd_igl_const;             //!< Constant used in speed-density algorithm to calculate PW. Const = ((CYL_DISP * 3.482 * 18750000) / Ifr ) * ((Nbnk * Ncyl) / (Nsq * Ninj))

    uint16_t inj_prime_cold;               //!< Prime pulse PW at cold (CLT=-30°C)
    uint16_t inj_prime_hot;                //!< Prime pulse PW at hot (CLT=70°C)
    uint8_t  inj_prime_delay;              //!< Prime pulse delay in 0.1 sec units

    uint16_t inj_cranktorun_time;          //!< Time in seconds for going from the crank position to the run position (1 tick = 10ms)
    uint8_t  inj_aftstr_strokes;           //!< Number of engine strokes, during this time afterstart enrichment is applied (divided by 2)

    uint8_t  inj_lambda_str_per_stp;       //!< Number of strokes per step for lambda control
    uint8_t  inj_lambda_step_size_p;       //!< "+" Step size, value * 512, max 0.49
    uint16_t inj_lambda_corr_limit_p;      //!< "+" limit, value * 512
    uint16_t inj_lambda_swt_point;         //!< lambda switch point in volts
    int16_t  inj_lambda_temp_thrd;         //!< Coolant temperature activation threshold
    uint16_t inj_lambda_rpm_thrd;          //!< RPM activation threshold
    uint8_t  inj_lambda_activ_delay;       //!< Lambda sensor activation delay

    uint8_t  inj_ae_tpsdot_thrd;           //!< TPS %/sec threshold, max rate is 255%/sec
    uint8_t  inj_ae_coldacc_mult;          //!< Cold acceleration multiplier (-30°C), (value - 1.0) * 128

    uint16_t gd_steps;                     //!< Number of steps of gas dosator stepper motor

    int16_t  inj_timing;                   //!< Injection timing in crank degrees * ANGLE_MULTIPLIER

    uint8_t  flpmp_flags;                  //!< fuel pump flags

    uint8_t  choke_flags;                  //!< choke related flags (see CKF_x constants for more information)

    uint16_t revlim_lot;                   //!< lower threshold for rev.limitting (fuel injection)
    uint16_t revlim_hit;                   //!< upper threshold for rev.limitting (fuel injection)

    int16_t  inj_timing_crk;               //!< Injection timing on cranking in crank degrees * ANGLE_MULTIPLIER

    uint8_t  gd_fc_closing;                //!< How much close (in %) gas doser in fuel cut mode (relatively to current position)

    uint8_t  inj_lambda_step_size_m;       //!<"-" Step size, value * 512, max 0.49

    uint16_t inj_lambda_corr_limit_m;      //!<"-" limit, value * 512

    uint16_t gd_lambda_corr_limit_p;       //!<"+" limit, used for gas doser (idling), value * 512
    uint16_t gd_lambda_corr_limit_m;       //!<"-" limit, used for gas doser (idling), value * 512

    uint16_t inj_lambda_dead_band;         //!< lambda switch point dead band

    uint8_t  load_src_cfg;                 //!< Engine load source selection (0 - MAP, 1 - TPS)

    /**Following reserved bytes required for keeping binary compatibility between
     * different versions of firmware. Useful when you add/remove members to/from
     * this structure. */
    uint8_t  reserved[63];

    /**CRC of this structure (for checking correctness of data after loading from EEPROM) */
    uint16_t crc;
}params_t;

typedef struct sensors_t
{
    uint16_t map;                           //!< Intake Manifold Pressure (давление во впускном коллекторе (усредненное))
    uint16_t voltage;                       //!< Board voltage (напряжение бортовой сети (усредненное))
    int16_t  temperat;                      //!< Coolant temperature (температура охлаждающей жидкости (усредненная))
    uint16_t frequen;                       //!< Averaged RPM (частота вращения коленвала (усредненная))
    uint16_t inst_frq;                      //!< Instant RPM - not averaged  (мгновенная частота вращения)
    uint8_t  carb;                          //!< State of carburetor's limit switch (состояние концевика карбюратора)
    uint8_t  gas;                           //!< State of gas valve (состояние газового клапана)
    uint16_t knock_k;                       //!< Knock signal level (уровень сигнала детонации)
    uint8_t  tps;                           //!< Throttle position sensor (0...100%, x2)
    uint16_t add_i1;                        //!< ADD_I1 input voltage
    uint16_t add_i2;                        //!< ADD_I2 input voltage

    uint16_t speed;                         //!< Vehicle speed expressed by period between speed sensor pulses (1 tick = 4us)
    uint32_t distance;                      //!< Distance expressed by number of speed sensor pulses since last ignition turn on

    int16_t air_temp;                       //!< Intake air temperature

    uint16_t pa4;                           //!< PA4 input voltage

    int16_t tpsdot;                         //!< Speed of TPS movement (d%/dt = %/s), positive when acceleration, negative when deceleration

    //сырые значения датчиков (дискреты АЦП с компенсированными погрешностями)
    int16_t  map_raw;                       //!< raw ADC value from MAP sensor
    int16_t  voltage_raw;                   //!< raw ADC value from voltage
    int16_t  temperat_raw;                  //!< raw ADC value from coolant temperature sensor
    int16_t  tps_raw;                       //!< raw ADC value from TPS sensor
    int16_t  add_i1_raw;                    //!< raw ADC value from ADD_I1 input
    int16_t  add_i2_raw;                    //!< raw ADC value from ADD_I2 input
}sensors_t;

typedef struct correct_t
{
    int16_t curr_angle;                     //!< Current advance angle (текущий угол опережения)
    int16_t knock_retard;                   //!< Correction of advance angle from knock detector (поправка УОЗ от регулятора по детонации)
    int16_t idlreg_aac;                     //!< Idle regulator advance angle correction
    int16_t octan_aac;                      //!< Octane advance angle correction
    int16_t strt_aalt;                      //!< Advance angle from start map
    int16_t idle_aalt;                      //!< Advance angle from idle map
    int16_t work_aalt;                      //!< Advance angle from work map
    int16_t temp_aalt;                      //!< Advance angle from coolant temp. corr. map
    int16_t airt_aalt;                      //!< Advance angle from air temp. corr. map

    int16_t lambda;                         //!< Current value of lambda (EGO) correction, can be negative

    uint8_t afr;                            //!< Current value of air to fuel ratio (from AFR map)
    int16_t inj_timing;                     //!< Current injection timing

    int16_t pa4_aac;                        //!< Ignition timing correction from PA4

}correct_t;

typedef struct f_data_t
{
    uint8_t name[F_NAME_SIZE];                        //!< associated name of tables' set, displayed in user interface
    //ignition maps
    int8_t f_str[F_STR_POINTS];                       //!< function of advance angle at start
    int8_t f_idl[F_IDL_POINTS];                       //!< function of advance angle at idling
    int8_t f_wrk[F_WRK_POINTS_L][F_WRK_POINTS_F];     //!< working function of advance angle
    int8_t f_tmp[F_TMP_POINTS];                       //!< coolant temper. correction of advance angle
    //fuel injection maps
    uint8_t inj_ve[INJ_VE_POINTS_L][INJ_VE_POINTS_F]; //!< Volumetric efficiency lookup table, value * 128
    uint8_t inj_afr[INJ_VE_POINTS_L][INJ_VE_POINTS_F];//!< Air-Fuel ratio lookup table, (1/value) * 2048, e.g. 1/14.7 * 2048 = 139
    uint16_t inj_cranking[INJ_CRANKING_LOOKUP_TABLE_SIZE];//!< Injector pulse width used when engine is starting up (cranking)
    uint8_t inj_warmup[INJ_WARMUP_LOOKUP_TABLE_SIZE]; //!< Warmup enrichment lookup table (factor), value * 128, e.g. 128 = 1.00
    uint16_t inj_dead_time[INJ_DT_LOOKUP_TABLE_SIZE]; //!< Injector dead-time lookup table, value in ticks of timer, 1 tick = 3.2uS
    /**Position of the IAC/PWM vs coolant temperature for run mode (used in open-loop idle control)
     * value in % * 2, e.g. 200 = 100.0% */
    uint8_t inj_iac_run_pos[INJ_IAC_POS_TABLE_SIZE];
    /**Position of the IAC/PWM vs coolant temperature for cranking mode (used by both in open and closed-loop idle control)
     * value in % * 2, e.g. 200 = 100.0% */
    uint8_t inj_iac_crank_pos[INJ_IAC_POS_TABLE_SIZE];
    //note! inj_ae_tps_enr must be followed by inj_ae_tps_bins, inj_ae_rpm_enr must be followed by inj_ae_rpm_bins
    uint8_t inj_ae_tps_enr[INJ_AE_TPS_LOOKUP_TABLE_SIZE];  //!< values of the AE's TPS lookup table (additive factor), value + 55, e.g. 155 = 1.00, this means AE = 100% (so PW will be increased by 100%))
    int8_t  inj_ae_tps_bins[INJ_AE_TPS_LOOKUP_TABLE_SIZE]; //!< bins of the AE's TPS lookup table (d%/dt, (signed value in %) / 100ms)
    uint8_t inj_ae_rpm_enr[INJ_AE_RPM_LOOKUP_TABLE_SIZE];  //!< values of the AE's RPM lookup table (factor), value * 128.0, e.g. 128 = 1.00, this means to use 100% of AE)
    uint8_t inj_ae_rpm_bins[INJ_AE_RPM_LOOKUP_TABLE_SIZE]; //!< bins of the AE's RPM lookup table (value / 100, e.g. value=25 means 2500min-1)

    uint8_t inj_aftstr[INJ_AFTSTR_LOOKUP_TABLE_SIZE];      //!< afterstart enrichment vs coolant temperature lookup table, value * 128.0, 128 = 1.00 and means 100% will be adde to fuel

    uint8_t inj_timing[INJ_VE_POINTS_L][INJ_VE_POINTS_F];  //!< injection timing in crankshaft degrees (value / 3.0), 0...720 deg.

    /* Following reserved bytes required for keeping binary compatibility between
     * different versions of firmware. Useful when you add/remove members to/from
     * this structure. */
    uint8_t reserved[446];
}f_data_t;

typedef struct fw_ex_data_t
{
    /**Knock. table of attenuator's gain factors (contains codes of gains, gain depends on RPM) */
    uint8_t attenuator_table[KC_ATTENUATOR_LOOKUP_TABLE_SIZE];

    /**Table for dwell control. Accumulation(dwell) time depends on board voltage */
    uint16_t coil_on_time[COIL_ON_TIME_LOOKUP_TABLE_SIZE];

    /**Coolant temperature sensor lookup table */
    int16_t cts_curve[THERMISTOR_LOOKUP_TABLE_SIZE];
    /**Voltage corresponding to the beginning of axis*/
    uint16_t cts_vl_begin;
    /**Voltage corresponding to the end of axis*/
    uint16_t cts_vl_end;

    /**Choke closing versus coolant temperature */
    uint8_t choke_closing[CHOKE_CLOSING_LOOKUP_TABLE_SIZE];

    /**Air temperature sensor lookup table*/
    int16_t ats_curve[THERMISTOR_LOOKUP_TABLE_SIZE];
    /**Voltage corresponding to the beginning of axis*/
    uint16_t ats_vl_begin;
    /**Voltage corresponding to the end of axis*/
    uint16_t ats_vl_end;

    /**Air temperature correction of advance angle*/
    int8_t ats_corr[ATS_CORR_LOOKUP_TABLE_SIZE];

    /**Points of the RPM grid*/
    int16_t rpm_grid_points[RPM_GRID_SIZE];
    /**Sizes of cells in RPM grid (so, we don't need to calculate them at the runtime)*/
    int16_t rpm_grid_sizes[RPM_GRID_SIZE-1];

    /** Gas dose actuator position vs (TPS,RPM)*/
    uint8_t gasdose_pos[GASDOSE_POS_TPS_SIZE][GASDOSE_POS_RPM_SIZE];

    /**Following reserved bytes required for keeping binary compatibility between
     * different versions of firmware. Useful when you add/remove members to/from
     * this structure. */
    uint8_t reserved[1792];
}fw_ex_data_t;

/**Describes system's data (main ECU data structure)
 * описывает данные системы, обеспечивает единый интерфейс данных
 */
typedef struct ecudata_t
{
    struct params_t  param;                 //!< --parameters (параметры)
    struct sensors_t sens;                  //!< --sensors (сенсоры)
    struct correct_t corr;                  //!< --calculated corrections and lookup tables' values

    uint8_t  ie_valve;                      //!< State of Idle cut off valve (состояние клапана ЭПХХ)
    uint8_t  fe_valve;                      //!< State of Power valve (состояние клапана ЭМР)

    uint8_t  fc_revlim;                     //!< Flag indicates fuel cut from rev. limitter

    uint8_t  cool_fan;                      //!< State of the cooling fan (состояние электровентилятора)
    uint8_t  st_block;                      //!< State of the starter blocking output (состояние выхода блокировки стартера)
    uint8_t  ce_state;                      //!< State of CE lamp (состояние лампы "CE")
    uint8_t  airflow;                       //!< Air flow (расход воздуха)
    uint8_t  choke_pos;                     //!< Choke position in % * 2
    uint8_t  gasdose_pos;       /*GD*/      //!< Gas dosator position in % * 2

    f_data_t tables_ram;                    //!< set of tables in RAM
    //mm_func8_ptr_t mm_ptr8;                 //!< callback, returns 8 bit result, unsigned
    //mm_func16_ptr_t mm_ptr16;               //!< callback, return 16 bit result, unsigned

    f_data_t *fn_dat;                  //!< Pointer to the set of tables (указатель на набор характеристик)

    uint16_t op_comp_code;                  //!< Contains code of operation for packet being sent - OP_COMP_NC (содержит код который посылается через UART (пакет OP_COMP_NC))
    uint16_t op_actn_code;                  //!< Contains code of operation for packet being received - OP_COMP_NC (содержит код который принимается через UART (пакет OP_COMP_NC))
    uint16_t ecuerrors_for_transfer;        //!< Buffering of error codes being sent via UART in real time (буферизирует коды ошибок передаваемые через UART в реальном времени)
    uint16_t ecuerrors_saved_transfer;      //!< Buffering of error codes for read/write from/to EEPROM which is being sent/received (буферизирует коды ошибок для чтения/записи в EEPROM, передаваемые/принимаемые через UART)
    uint8_t  use_knock_channel_prev;        //!< Previous state of knock channel's usage flag (предыдущее состояние признака использования канала детонации)

    uint8_t engine_mode;                    //!< Current engine mode(start, idle, work) (текущий режим двигателя (пуск, ХХ, нагрузка))

    uint8_t choke_testing;                  //!< Used to indcate that choke testing is on/off (so it is applicable only if SM_CONTROL compilation option is used)
    int8_t choke_manpos_d;                  //!< Muanual position setting delta value used for choke control
    uint8_t choke_rpm_reg;                  //!< Used to indicate that at the moment system regulates RPM by means of choke position

    uint8_t gasdose_testing;    /*GD*/      //!< Used to indcate that gas dosator testing is on/off (so it is applicable only if GD_CONTROL compilation option is used)
    int8_t gasdose_manpos_d;    /*GD*/      //!< Muanual position setting delta value used for gasdose control

    uint8_t bt_name[9];                     //!< received name for Bluetooth (8 chars max), zero element is size
    uint8_t bt_pass[7];                     //!< received password for Bluetooth (6 chars max), zero element is size
    uint8_t sys_locked;                     //!< used by immobilizer to indicate that system is locked

    uint16_t inj_pw;                        //!< current value of injector pulse width

    uint8_t acceleration;                   //!< acceleration/decelaration flag

}ecudata_t;

typedef uint16_t fnptr_t;                //!< Special type for function pointers
#define IOREM_SLOTS  37                  //!< Number of slots used for I/O remapping
#define IOREM_PLUGS  68                  //!< Number of plugs used in I/O remapping

/**Describes all data related to I/O remapping */
typedef struct iorem_slots_t
{
    fnptr_t i_slots[IOREM_SLOTS];           //!< initialization slots
    fnptr_t i_slotsi[IOREM_SLOTS];          //!< initialization slots (inverted)
    fnptr_t v_slots[IOREM_SLOTS];           //!< data slots
    fnptr_t v_slotsi[IOREM_SLOTS];          //!< data slots           (inverted)
    fnptr_t i_plugs[IOREM_PLUGS];           //!< initialization plugs
    fnptr_t v_plugs[IOREM_PLUGS];           //!< data plugs
    fnptr_t s_stub;                         //!< special pointer used as stub
    fnptr_t g_stub;                         //!< reserved
    uint8_t version;                        //!< version of this structure (used for compatibility checkings)
    uint16_t size;                          //!< size of this structure (used for compatibility checkings)
}iorem_slots_t;

typedef struct cd_data_t
{
    /** Arrays which are used for I/O remapping. Some arrays are "slots", some are "plugs" */
    iorem_slots_t iorem;

    /**Holds flags which give information about options were used to build firmware */
    uint32_t config;

    uint8_t reserved[3];                    //!< reserved bytes

    uint8_t fw_version;                     //!< version of the firmware

    uint16_t size;                          //!< size of this structure (used for compatibility checkings)
}cd_data_t;

typedef struct fw_data_t
{
    cd_data_t cddata;                       //!< All data which is strongly coupled with code

    //following fields are belong to data area, not to the code area:
    params_t def_param;                     //!< Reserve parameters (loaded when instance in EEPROM is broken)

    fw_ex_data_t exdata;                    //!< Additional data containing separate tables

    f_data_t tables[TABLES_NUMBER_PGM];     //!< Tables' sets for advance angle and fuel injection

    uint8_t fw_signature_info[FW_SIGNATURE_INFO_SIZE];//!< Signature information (contains information about firmware)

    uint8_t version;                        //!< version of this structure

    uint16_t fw_data_size;                  //!< Used for checking compatibility with mngmt software. Holds size of all data stored in the firmware.

    uint16_t code_crc;                      //!< Check sum of the whole firmware (except this check sum and boot loader)
}fw_data_t;

//////////////////////////////////////////////////////////////////////////////////////////

#define  UART_RECV_BUFF_SIZE     82     //!< Size of receiver's buffer
#define  UART_SEND_BUFF_SIZE     112    //!< Size of transmitter's buffer

typedef struct
{
    uint8_t send_mode;                     //!< current descriptor of packets beeing send
    uint8_t recv_buf[UART_RECV_BUFF_SIZE]; //!< receiver's buffer
    uint8_t send_buf[UART_SEND_BUFF_SIZE]; //!< transmitter's buffer
    volatile uint8_t send_size;            //!< size of data to be send
    uint8_t send_index;                    //!< index in transmitter's buffer
    volatile uint8_t recv_size;            //!< size of received data
    uint8_t recv_index;                    //!< index in receiver's buffer
    int tblIdx;
    uint8_t state, wrk_index;
    uint8_t index;

}uartstate_t;


//////////////////////////////////////////////////////////////////////////////////////////


#define   CHANGEMODE   'h'   //!< change mode (type of default packet)
#define   BOOTLOADER   'i'   //!< start boot loader

#define   TEMPER_PAR   'j'   //!< temperature parameters (coolant sensor, engine cooling etc)
#define   CARBUR_PAR   'k'   //!< carburetor's parameters
#define   IDLREG_PAR   'l'   //!< idling regulator parameters
#define   ANGLES_PAR   'm'   //!< advance angle (ign. timing) parameters
#define   FUNSET_PAR   'n'   //!< parametersrelated to set of functions (lookup tables)
#define   STARTR_PAR   'o'   //!< engine start parameters

#define   FNNAME_DAT   'p'   //!< used for transfering of names of set of functions (lookup tables)
#define   SENSOR_DAT   'q'   //!< used for transfering of sensors data

#define   ADCCOR_PAR   'r'   //!< parameters related to ADC corrections
#define   ADCRAW_DAT   's'   //!< used for transfering 'raw' values directly from ADC

#define   CKPS_PAR     't'   //!< CKP sensor parameters
#define   OP_COMP_NC   'u'   //!< used to indicate that specified (suspended) operation completed

#define   CE_ERR_CODES 'v'   //!< used for transfering of CE codes

#define   KNOCK_PAR    'w'   //!< parameters related to knock detection and knock chip

#define   CE_SAVED_ERR 'x'   //!< used for transfering of CE codes stored in the EEPROM
#define   FWINFO_DAT   'y'   //!< used for transfering information about firmware
#define   MISCEL_PAR   'z'   //!< miscellaneous parameters
#define   EDITAB_PAR   '{'   //!< used for transferring of data for realtime tables editing
#define   ATTTAB_PAR   '}'   //!< used for transferring of attenuator map (knock detection related)
#define   RPMGRD_PAR   '"'   //!< used for transferring of RPM grid
#define   DBGVAR_DAT   ':'   //!< for watching of firmware variables (used for debug purposes)
#define   DIAGINP_DAT  '='   //!< diagnostics: send input values (analog & digital values)
#define   DIAGOUT_DAT  '^'   //!< diagnostics: receive output states (bits)

#define   CHOKE_PAR    '%'   //!< parameters  related to choke control
#define   SECUR_PAR    '#'   //!< security parameters
#define   UNIOUT_PAR   '&'   //!< universal tunable outputs' parameters

#define   INJCTR_PAR   ';'   //!< fuel injection configuration parameters
#define   LAMBDA_PAR   '-'   //!< lambda correction parameters
#define   ACCEL_PAR    '|'   //!< acceleration enrichment parameters

#define   GASDOSE_PAR  '*'   //!< gas dose parameters
#define   SIGINF_DAT   '~'   //! signature information


#define ETMT_NAME_STR 0     //!< name of tables's set id
//ignition maps
#define ETMT_STRT_MAP 1     //!< start map id
#define ETMT_IDLE_MAP 2     //!< idle map id
#define ETMT_WORK_MAP 3     //!< work map id
#define ETMT_TEMP_MAP 4     //!< temp.corr. map id
//fuel injection maps
#define ETMT_VE_MAP   5     //!< VE
#define ETMT_AFR_MAP  6     //!< AFR
#define ETMT_CRNK_MAP 7     //!< Cranking PW
#define ETMT_WRMP_MAP 8     //!< Warmup enrichment
#define ETMT_DEAD_MAP 9     //!< Injector's dead time
#define ETMT_IDLR_MAP 10    //!< IAC/PWM position on run
#define ETMT_IDLC_MAP 11    //!< IAC_PWM position on cranking
#define ETMT_AETPS_MAP 12   //!< AE TPS map
#define ETMT_AERPM_MAP 13   //!< AE RPM map
#define ETMT_AFTSTR_MAP 14  //!< afterstart enrichment
#define ETMT_IT_MAP   15    //!< injection timing map




static const int PLATFORM_MN_SIZE = 4; //!< Magic number identifying platform (holds last 4 bytes in FLASH)

#define EEPROM_PARAM_START 0x0001
#define EEPROM_ECUERRORS_START (EEPROM_PARAM_START+(sizeof(params_t)))
#define EEPROM_REALTIME_TABLES_START (EEPROM_ECUERRORS_START + 5)

typedef enum EECUPlatform
{
    EP_ATMEGA16 = 0,
    EP_ATMEGA32,
    EP_ATMEGA64,
    EP_ATMEGA128,
    EP_ATMEGA644,            //redundant to EP_ATMEGA64 by firmware and EEPROM sizes
    EP_NR_OF_PLATFORMS       //must be last!
};

struct PPFlashParam
{
    size_t m_page_size;       //size of program memory page in bytes
    size_t m_page_count;      //number of pages
    size_t m_total_size;      //общий размер памяти программ микроконтроллера
    size_t m_bl_section_size; //кол-во байт отведенное для бутлоадера (из секции бутлоадера этот блок можно читать)
    size_t m_app_section_size;//часть прошивки кроме бутлоадера
    size_t m_fcpu_hz;         //MCU clock frequency (e.g. 16000000)
    char   m_magic[PLATFORM_MN_SIZE]; //Magic number identifying platform (holds last 4 bytes in FLASH)
    EECUPlatform m_platform_id;//platform ID
};

struct PPEepromParam
{
    size_t m_size;             //size of EEPROM
    EECUPlatform m_platform_id;//platform ID
};

#define TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER 4
#define ROUND(x) ((int16_t)( (x) + 0.5 - ((x) < 0) ))
#define TEMPERATURE_MAGNITUDE(t) ROUND ((t) * TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER)

////////////////////////////////////////////
// some stuff for S3EMUL...

const uint8_t fwinfo[61] = " S3EMUL C 2017 andreika, based on SECU-3, http://secu-3.org ";

// #define COPT_REALTIME_TABLES 9
// #define COPT_FUEL_INJECT 28
// #define COPT_GD_CONTROL 29

typedef struct
{
    int mapIndex;
    int mapAddress;
    int mapSize;

    int opCode;
    int opData;

    int modeId;
} PacketParams;

typedef struct ecudata_t D;
typedef struct fw_data_t F;


#pragma pack(pop)
