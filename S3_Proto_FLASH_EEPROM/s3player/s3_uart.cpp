/* s3emul - "secu3 emulator"
* Copyright (C) 2016 andreika. Ukraine, Kiev
*
* The following code is a derivative work of the code from the SECU-3 project,
* which is licensed GPLv3. This code therefore is also licensed under the terms
* of the GNU Public License, verison 3.
*
* PORTIONS OF THIS CODE TAKEN FROM:
*
* SECU-3  - An open source, free engine control unit
* Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* contacts:
*            http://secu-3.org
*            email: shabelnikov@secu-3.org
*/

#include <stdio.h>
#include <stdlib.h>

#include "s3emul.h"
#include "s3.h"

// There are several special reserved symbols in binary mode: 0x21, 0x40, 0x0D, 0x0A
#define FIBEGIN  0x21       //!< '!' indicates beginning of the ingoing packet
#define FOBEGIN  0x40       //!< '@' indicates beginning of the outgoing packet
#define FIOEND   0x0D       //!<'\r' indicates ending of the ingoing/outgoing packet
#define FESC     0x0A       //!<'\n' Packet escape (FESC)
// Following bytes are used only in escape sequeces and may appear in the data without any problems
#define TFIBEGIN 0x81       //!< Transposed FIBEGIN
#define TFOBEGIN 0x82       //!< Transposed FOBEGIN
#define TFIOEND  0x83       //!< Transposed FIOEND
#define TFESC    0x84       //!< Transposed FESC


inline void append_tx_buff(uartstate_t &uart, uint8_t b)
{
    if (b == FOBEGIN)
    {
        uart.send_buf[uart.send_size++] = FESC;
        uart.send_buf[uart.send_size++] = TFOBEGIN;
    }
    else if ((b) == FIOEND)
    {
        uart.send_buf[uart.send_size++] = FESC;
        uart.send_buf[uart.send_size++] = TFIOEND;
    }
    else if ((b) == FESC)
    {
        uart.send_buf[uart.send_size++] = FESC;
        uart.send_buf[uart.send_size++] = TFESC;
    }
    else
        uart.send_buf[uart.send_size++] = b;
}

inline uint8_t takeout_rx_buff(uartstate_t &uart)
{
    uint8_t b1 = uart.recv_buf[uart.recv_index++];
    if (b1 == FESC)
    {
        uint8_t b2 = uart.recv_buf[uart.recv_index++];
        if (b2 == TFIBEGIN)
            return FIBEGIN;
        else if (b2 == TFIOEND)
            return FIOEND;
        else if (b2 == TFESC)
            return FESC;
        return 0; //wrong code
    }
    else
        return b1;
}

/////////////////////////////////////////////////////////////////////////////////

inline void _build_i16h(uartstate_t &uart, uint16_t i)
{
    append_tx_buff(uart, _AB((i), 1));
    append_tx_buff(uart, _AB((i), 0));
}

inline void _build_i24h(uartstate_t &uart, uint32_t i)
{
    append_tx_buff(uart, (i >> 16));
    _build_i16h(uart, i);
}

inline void _build_i32h(uartstate_t &uart, uint32_t i)
{
    _build_i16h(uart, i >> 16);
    _build_i16h(uart, i);
}

#define build_i4h(i)  append_tx_buff(uart, (i))
#define build_i8h(i)  append_tx_buff(uart, (i))

#define build_i16h(i) _build_i16h(uart, (i))
#define build_i24h(i) _build_i24h(uart, (i))
#define build_i32h(i) _build_i32h(uart, (i))

#define build_fs(buf, size) build_buf8(uart, (buf), (size))
#define build_fb(buf, size) build_buf8(uart, (buf), (size))
#define build_rs(buf, size) build_buf8(uart, (buf), (size))
#define build_rb(buf, size) build_buf8(uart, (buf), (size))
#define build_rw(buf, size) build_buf16(uart, (buf), (size))

inline void build_buf8(uartstate_t &uart, uint8_t *buf, uint8_t size)
{
    while(size--) build_i8h(*buf++);
}

inline void build_buf16(uartstate_t &uart, const uint16_t *buf, uint8_t size)
{
    while(size--) build_i16h(*buf++);
}

////////////////////////////////////////////////////////////////////////////////

inline uint16_t _recept_i16h(uartstate_t &uart)
{
    uint16_t i16;
    _AB(i16, 1) = takeout_rx_buff(uart);
    _AB(i16, 0) = takeout_rx_buff(uart);
    return i16;
}

static uint32_t _recept_i24h(uartstate_t &uart)
{
    uint32_t i = 0;
    i = takeout_rx_buff(uart);
    i = i << 16;
    i |= _recept_i16h(uart);
    return i;
}

static uint32_t _recept_i32h(uartstate_t &uart)
{
    uint32_t i = 0;
    i = _recept_i16h(uart);
    i = i << 16;
    i |= _recept_i16h(uart);
    return i;
}

inline void _recept_buf8(uartstate_t &uart, uint8_t* ramBuffer, uint8_t size)
{
    while (size-- && uart.recv_index < uart.recv_size) *ramBuffer++ = takeout_rx_buff(uart);
}

inline void _recept_buf16(uartstate_t &uart, uint16_t* ramBuffer, uint8_t size)
{
    while (size-- && uart.recv_index < uart.recv_size) *ramBuffer++ = _recept_i16h(uart);
}

#define recept_i4h() takeout_rx_buff(uart)
#define recept_i8h() takeout_rx_buff(uart)
#define recept_i16h() _recept_i16h(uart)
#define recept_i24h() _recept_i24h(uart)
#define recept_i32h() _recept_i32h(uart)

#define recept_rb(buf, size) _recept_buf8(uart, buf, size)
#define recept_rs(buf, size) _recept_buf8(uart, buf, size)
#define recept_rw(buf, size) _recept_buf16(uart, buf, size)

////////////////////////////////////////////////////////////////////////////////

void send_s3_client_packet(uartstate_t &uart, struct ecudata_t* d, struct fw_data_t &fw_data, uint8_t send_mode)
{
    uart.send_size = 0;
    if (send_mode==0) send_mode = uart.send_mode;
    uart.send_buf[uart.send_size++] = '@';
    uart.send_buf[uart.send_size++] = send_mode;
    #ifdef UART_DEBUG
    printf("send_mode >%c\n", send_mode);
    #endif
    switch(send_mode)
    {
        case TEMPER_PAR:
            build_i4h(d->param.tmp_use);
            build_i4h(d->param.vent_pwm);
            build_i4h(d->param.cts_use_map);
            build_i16h(d->param.vent_on);
            build_i16h(d->param.vent_off);
            build_i16h(d->param.vent_pwmfrq);
            break;

        case CARBUR_PAR:
            build_i16h(d->param.ie_lot);
            build_i16h(d->param.ie_hit);
            build_i4h(d->param.carb_invers);
            build_i16h(d->param.fe_on_threshold);
            build_i16h(d->param.ie_lot_g);
            build_i16h(d->param.ie_hit_g);
            build_i8h(d->param.shutoff_delay);
            build_i8h(d->param.tps_threshold);
            build_i16h(d->param.fuelcut_map_thrd);
            build_i16h(d->param.fuelcut_cts_thrd);
            build_i16h(d->param.revlim_lot);
            build_i16h(d->param.revlim_hit);
            break;

        case IDLREG_PAR:
            build_i8h(d->param.idl_flags);   //idling flags
            build_i16h(d->param.ifac1);
            build_i16h(d->param.ifac2);
            build_i16h(d->param.MINEFR);
            build_i16h(d->param.idling_rpm);
            build_i16h(d->param.idlreg_min_angle);
            build_i16h(d->param.idlreg_max_angle);
            build_i16h(d->param.idlreg_turn_on_temp);
            break;

        case ANGLES_PAR:
            build_i16h(d->param.max_angle);
            build_i16h(d->param.min_angle);
            build_i16h(d->param.angle_corr);
            build_i16h(d->param.angle_dec_speed);
            build_i16h(d->param.angle_inc_speed);
            build_i4h(d->param.zero_adv_ang);
            break;

        case FUNSET_PAR:
            build_i8h(d->param.fn_gasoline);
            build_i8h(d->param.fn_gas);
            build_i16h(d->param.map_lower_pressure);
            build_i16h(d->param.map_upper_pressure);
            build_i16h(d->param.map_curve_offset);
            build_i16h(d->param.map_curve_gradient);
            build_i16h(d->param.tps_curve_offset);
            build_i16h(d->param.tps_curve_gradient);
            build_i4h(d->param.load_src_cfg);
            break;

        case STARTR_PAR:
            build_i16h(d->param.starter_off);
            build_i16h(d->param.smap_abandon);
            build_i16h(d->param.inj_cranktorun_time); //fuel injection
            build_i8h(d->param.inj_aftstr_strokes);   //fuel injection
            build_i16h(d->param.inj_prime_cold);      //fuel injection
            build_i16h(d->param.inj_prime_hot);       //fuel injection
            build_i8h(d->param.inj_prime_delay);      //fuel injection
            break;

        case FNNAME_DAT:
            build_i8h(TABLES_NUMBER);
            build_i8h(uart.index);
            build_fs((uart.index < TABLES_NUMBER_PGM ? fw_data.tables[uart.index] : d->tables_ram).name, F_NAME_SIZE);
            ++uart.index;
            if (uart.index >= (TABLES_NUMBER)) uart.index = 0;
            break;

        case SENSOR_DAT:
            build_i16h(d->sens.frequen);           // averaged RPM
            build_i16h(d->sens.map);               // averaged MAP pressure
            build_i16h(d->sens.voltage);           // voltage (avaraged)
            build_i16h(d->sens.temperat);          // coolant temperature
            build_i16h(d->corr.curr_angle);        // advance angle
            build_i16h(d->sens.knock_k);           // knock value
            build_i16h(d->corr.knock_retard);      // knock retard
            build_i8h(d->airflow);                 // index of the map axis curve
            //boolean values
            build_i8h((d->ie_valve   << 0) |       // IE flag
            (d->sens.carb  << 1) |       // carb. limit switch flag
            (d->sens.gas   << 2) |       // gas valve flag
            (d->fe_valve   << 3) |       // power valve flag
            (d->ce_state   << 4) |       // CE flag
            (d->cool_fan   << 5) |       // cooling fan flag
            (d->st_block   << 6) |       // starter blocking flag
            (d->acceleration << 7));     // acceleration enrichment flag
            build_i8h(d->sens.tps);                // TPS (0...100%, x2)
            build_i16h(d->sens.add_i1);            // averaged ADD_I1 voltage
            build_i16h(d->sens.add_i2);            // ADD_I2 voltage
            build_i16h(d->ecuerrors_for_transfer); // CE errors
            build_i8h(d->choke_pos);               // choke position
            build_i8h(d->gasdose_pos);             // gas dosator position
            build_i16h(d->sens.speed);             // vehicle speed (2 bytes)
            build_i24h(d->sens.distance);          // distance (3 bytes)
            build_i16h(d->sens.air_temp);

            //corrections
            build_i16h(d->corr.strt_aalt);         // advance angle from start map
            build_i16h(d->corr.idle_aalt);         // advance angle from idle map
            build_i16h(d->corr.work_aalt);         // advance angle from work map
            build_i16h(d->corr.temp_aalt);         // advance angle from coolant temperature correction map
            build_i16h(d->corr.airt_aalt);         // advance angle from air temperature correction map
            build_i16h(d->corr.idlreg_aac);        // advance angle correction from idling RPM regulator
            build_i16h(d->corr.octan_aac);         // octane correction value

            build_i16h(d->corr.lambda);            // lambda correction
            build_i16h(d->inj_pw);                 // injector pulse width
            build_i16h(d->sens.tpsdot);            // TPS opening/closing speed

            break;

        case ADCCOR_PAR:
            build_i16h(d->param.map_adc_factor);
            build_i32h(d->param.map_adc_correction);
            build_i16h(d->param.ubat_adc_factor);
            build_i32h(d->param.ubat_adc_correction);
            build_i16h(d->param.temp_adc_factor);
            build_i32h(d->param.temp_adc_correction);
            //todo: In the future if we will have a lack of RAM we can split this packet into 2 pieces and decrease size of buffers
            build_i16h(d->param.tps_adc_factor);
            build_i32h(d->param.tps_adc_correction);
            build_i16h(d->param.ai1_adc_factor);
            build_i32h(d->param.ai1_adc_correction);
            build_i16h(d->param.ai2_adc_factor);
            build_i32h(d->param.ai2_adc_correction);
            break;

        case ADCRAW_DAT:
            build_i16h(d->sens.map_raw);
            build_i16h(d->sens.voltage_raw);
            build_i16h(d->sens.temperat_raw);
            build_i16h(d->sens.knock_k);   //<-- knock signal level
            build_i16h(d->sens.tps_raw);
            build_i16h(d->sens.add_i1_raw);
            build_i16h(d->sens.add_i2_raw);
            break;

        case CKPS_PAR:
            build_i4h(d->param.ckps_edge_type);
            build_i4h(d->param.ref_s_edge_type);
            build_i8h(d->param.ckps_cogs_btdc);
            build_i8h(d->param.ckps_ignit_cogs);
            build_i8h(d->param.ckps_engine_cyl);
            build_i4h(d->param.merge_ign_outs);
            build_i8h(d->param.ckps_cogs_num);
            build_i8h(d->param.ckps_miss_num);
            build_i8h(d->param.hall_flags);
            build_i16h(d->param.hall_wnd_width);
            break;

        case OP_COMP_NC:
            build_i16h(d->op_comp_code);
            break;

        case CE_ERR_CODES:
            build_i16h(d->ecuerrors_for_transfer);
            break;

        case KNOCK_PAR:
            build_i4h(d->param.knock_use_knock_channel);
            build_i8h(d->param.knock_bpf_frequency);
            build_i16h(d->param.knock_k_wnd_begin_angle);
            build_i16h(d->param.knock_k_wnd_end_angle);
            build_i8h(d->param.knock_int_time_const);

            build_i16h(d->param.knock_retard_step);
            build_i16h(d->param.knock_advance_step);
            build_i16h(d->param.knock_max_retard);
            build_i16h(d->param.knock_threshold);
            build_i8h(d->param.knock_recovery_delay);
            break;

        case CE_SAVED_ERR:
            build_i16h(d->ecuerrors_saved_transfer);
            break;

        case FWINFO_DAT:
            build_fs(fw_data.fw_signature_info, FW_SIGNATURE_INFO_SIZE);
            build_i32h(fw_data.cddata.config);   //<--compile-time options
            build_i8h(fw_data.cddata.fw_version); //<--version of the firmware
            break;

        case SIGINF_DAT:
            build_fs((uint8_t *)fwinfo, 60);
            break;

        case MISCEL_PAR:
            build_i16h(d->param.uart_divisor);
            build_i8h(d->param.uart_period_t_ms);
            build_i4h(d->param.ign_cutoff);
            build_i16h(d->param.ign_cutoff_thrd);
            build_i8h(d->param.hop_start_cogs);
            build_i8h(d->param.hop_durat_cogs);
            build_i8h(d->param.flpmp_flags);   //fuel pump flags
            break;

        case CHOKE_PAR:
            build_i16h(d->param.sm_steps);
            build_i4h(d->choke_testing);      //fake parameter (actually it is command)
            build_i8h(0);                     //fake parameter, not used in outgoing paket
            build_i8h(d->param.choke_startup_corr);
            build_i16h(d->param.choke_rpm[0]);
            build_i16h(d->param.choke_rpm[1]);
            build_i16h(d->param.choke_rpm_if);
            build_i16h(d->param.choke_corr_time);
            build_i16h(d->param.choke_corr_temp);
            build_i8h(d->param.choke_flags); //choke flags
            break;

        case GASDOSE_PAR:
            build_i16h(d->param.gd_steps);
            build_i4h(d->gasdose_testing);    //fake parameter (actually it is command)
            build_i8h(0);                     //fake parameter, not used in outgoing paket
            build_i8h(d->param.gd_fc_closing);
            build_i16h(d->param.gd_lambda_corr_limit_p);
            build_i16h(d->param.gd_lambda_corr_limit_m);
            break;

        case SECUR_PAR:
            build_i4h(0);
            build_i4h(0);
            build_i8h(d->param.bt_flags);
            build_rb(d->param.ibtn_keys[0], IBTN_KEY_SIZE);  //1st iButton key
            build_rb(d->param.ibtn_keys[1], IBTN_KEY_SIZE);  //2nd iButton key
            break;

        case UNIOUT_PAR:
        { //3 tunable outputs' parameters
            uint8_t oi = 0;
            for (; oi < UNI_OUTPUT_NUMBER; ++oi)
            {
                build_i8h(d->param.uni_output[oi].flags);
                build_i8h(d->param.uni_output[oi].condition1);
                build_i8h(d->param.uni_output[oi].condition2);
                build_i16h(d->param.uni_output[oi].on_thrd_1);
                build_i16h(d->param.uni_output[oi].off_thrd_1);
                build_i16h(d->param.uni_output[oi].on_thrd_2);
                build_i16h(d->param.uni_output[oi].off_thrd_2);
            }
            build_i4h(d->param.uniout_12lf);
            break;
        }

        case INJCTR_PAR:
            build_i8h(d->param.inj_flags);
            build_i8h(d->param.inj_config);
            build_i16h(d->param.inj_flow_rate);
            build_i16h(d->param.inj_cyl_disp);
            build_i32h(d->param.inj_sd_igl_const);
            build_i8h(d->param.ckps_engine_cyl);      //used for calculations on SECU-3 Manager side
            build_i16h(d->param.inj_timing);
            build_i16h(d->param.inj_timing_crk);
            break;

        case LAMBDA_PAR:
            build_i8h(d->param.inj_lambda_str_per_stp);
            build_i8h(d->param.inj_lambda_step_size_p);
            build_i8h(d->param.inj_lambda_step_size_m);
            build_i16h(d->param.inj_lambda_corr_limit_p);
            build_i16h(d->param.inj_lambda_corr_limit_m);
            build_i16h(d->param.inj_lambda_swt_point);
            build_i16h(d->param.inj_lambda_temp_thrd);
            build_i16h(d->param.inj_lambda_rpm_thrd);
            build_i8h(d->param.inj_lambda_activ_delay);
            build_i16h(d->param.inj_lambda_dead_band);
            break;

        case ACCEL_PAR:
            build_i8h(d->param.inj_ae_tpsdot_thrd);
            build_i8h(d->param.inj_ae_coldacc_mult);
            break;

        case EDITAB_PAR:
        {
            build_i8h(uart.state);  //map Id
            switch (uart.state)
            {
                case ETMT_STRT_MAP: //start map
                    build_i8h(0); //<--not used
                    build_rb((uint8_t*)&d->tables_ram.f_str, F_STR_POINTS);
                    uart.state = ETMT_IDLE_MAP;
                    break;
                case ETMT_IDLE_MAP: //idle map
                    build_i8h(0); //<--not used
                    build_rb((uint8_t*)&d->tables_ram.f_idl, F_IDL_POINTS);
                    uart.state = ETMT_WORK_MAP, uart.wrk_index = 0;
                    break;
                case ETMT_WORK_MAP: //work map
                    build_i8h(uart.wrk_index*F_WRK_POINTS_L);
                    build_rb((uint8_t*)&d->tables_ram.f_wrk[uart.wrk_index][0], F_WRK_POINTS_F);
                    if (uart.wrk_index >= F_WRK_POINTS_L - 1)
                    {
                        uart.wrk_index = 0;
                        uart.state = ETMT_TEMP_MAP;
                    }
                    else
                        ++uart.wrk_index;
                    break;
                case ETMT_TEMP_MAP: //temper. correction.
                    build_i8h(0); //<--not used
                    build_rb((uint8_t*)&d->tables_ram.f_tmp, F_TMP_POINTS);
                    uart.state = ETMT_NAME_STR;
                    break;
                case ETMT_NAME_STR:
                    build_i8h(0); //<--not used
                    build_rs(d->tables_ram.name, F_NAME_SIZE);
                    uart.state = ETMT_VE_MAP, uart.wrk_index = 0;
                    break;

                case ETMT_VE_MAP:
                    build_i8h(uart.wrk_index*INJ_VE_POINTS_L);
                    build_rb((uint8_t*)&d->tables_ram.inj_ve[uart.wrk_index][0], INJ_VE_POINTS_F);
                    if (uart.wrk_index >= INJ_VE_POINTS_L - 1)
                    {
                        uart.wrk_index = 0;
                        uart.state = ETMT_AFR_MAP;
                    }
                    else
                        ++uart.wrk_index;
                    break;
                case ETMT_AFR_MAP:
                    build_i8h(uart.wrk_index*INJ_VE_POINTS_L);
                    build_rb((uint8_t*)&d->tables_ram.inj_afr[uart.wrk_index][0], INJ_VE_POINTS_F);
                    if (uart.wrk_index >= INJ_VE_POINTS_L - 1)
                    {
                        uart.wrk_index = 0;
                        uart.state = ETMT_CRNK_MAP;
                    }
                    else
                        ++uart.wrk_index;
                    break;
                case ETMT_CRNK_MAP:
                    build_i8h(uart.wrk_index*(INJ_CRANKING_LOOKUP_TABLE_SIZE / 2));
                    build_rw((uint16_t*)&d->tables_ram.inj_cranking[uart.wrk_index*(INJ_CRANKING_LOOKUP_TABLE_SIZE / 2)], INJ_CRANKING_LOOKUP_TABLE_SIZE / 2);
                    if (uart.wrk_index >= 1)
                    {
                        uart.wrk_index = 0;
                        uart.state = ETMT_WRMP_MAP;
                    }
                    else
                        ++uart.wrk_index;
                    break;
                case ETMT_WRMP_MAP:
                    build_i8h(0); //<--not used
                    build_rb((uint8_t*)&d->tables_ram.inj_warmup, INJ_WARMUP_LOOKUP_TABLE_SIZE);
                    uart.state = ETMT_DEAD_MAP, uart.wrk_index = 0;
                    break;
                case ETMT_DEAD_MAP:
                    build_i8h(uart.wrk_index*(INJ_DT_LOOKUP_TABLE_SIZE / 4));
                    build_rw((uint16_t*)&d->tables_ram.inj_dead_time[uart.wrk_index*(INJ_DT_LOOKUP_TABLE_SIZE / 4)], (INJ_DT_LOOKUP_TABLE_SIZE / 4));
                    if (uart.wrk_index >= 3)
                    {
                        uart.wrk_index = 0;
                        uart.state = ETMT_IDLR_MAP;
                    }
                    else
                        ++uart.wrk_index;
                    break;
                case ETMT_IDLR_MAP:
                    build_i8h(0); //<--not used
                    build_rb((uint8_t*)&d->tables_ram.inj_iac_run_pos, INJ_IAC_POS_TABLE_SIZE);
                    uart.state = ETMT_IDLC_MAP;
                    break;
                case ETMT_IDLC_MAP:
                    build_i8h(0); //<--not used
                    build_rb((uint8_t*)&d->tables_ram.inj_iac_crank_pos, INJ_IAC_POS_TABLE_SIZE);
                    uart.state = ETMT_AETPS_MAP;
                    break;
                case ETMT_AETPS_MAP:
                    build_i8h(0); //<--not used
                    build_rb((uint8_t*)&d->tables_ram.inj_ae_tps_enr, INJ_AE_TPS_LOOKUP_TABLE_SIZE);
                    build_rb((uint8_t*)&d->tables_ram.inj_ae_tps_bins, INJ_AE_TPS_LOOKUP_TABLE_SIZE);
                    uart.state = ETMT_AERPM_MAP;
                    break;
                case ETMT_AERPM_MAP:
                    build_i8h(0); //<--not used
                    build_rb((uint8_t*)&d->tables_ram.inj_ae_rpm_enr, INJ_AE_RPM_LOOKUP_TABLE_SIZE);
                    build_rb((uint8_t*)&d->tables_ram.inj_ae_rpm_bins, INJ_AE_RPM_LOOKUP_TABLE_SIZE);
                    uart.state = ETMT_AFTSTR_MAP;
                    break;
                case ETMT_AFTSTR_MAP:
                    build_i8h(0); //<--not used
                    build_rb((uint8_t*)&d->tables_ram.inj_aftstr, INJ_AFTSTR_LOOKUP_TABLE_SIZE);
                    uart.state = ETMT_IT_MAP;
                    break;
                case ETMT_IT_MAP:
                    build_i8h(uart.wrk_index*INJ_VE_POINTS_L);
                    build_rb((uint8_t*)&d->tables_ram.inj_timing[uart.wrk_index][0], INJ_VE_POINTS_F);
                    if (uart.wrk_index >= INJ_VE_POINTS_L - 1)
                    {
                        uart.wrk_index = 0;
                        uart.state = ETMT_STRT_MAP;
                    }
                    else
                        ++uart.wrk_index;
                    break;
            }
            break;
        }

        //Transferring of RPM grid
                case RPMGRD_PAR:
                    build_i8h(0); //<--reserved
                    build_fb((uint8_t *)fw_data.exdata.rpm_grid_points, RPM_GRID_SIZE * sizeof(int16_t));
                    break;

                case ATTTAB_PAR:
                {
                    static uint8_t tab_index = 0;
                    build_i8h(tab_index * 16);
                    build_fb(&fw_data.exdata.attenuator_table[tab_index * 16], 16);
                    if (tab_index >= (KC_ATTENUATOR_LOOKUP_TABLE_SIZE / 16) - 1)
                        tab_index = 0;
                    else
                        ++tab_index;
                    break;
                }

                case DBGVAR_DAT:
                    build_i16h(0);
                    build_i16h(0);
                    build_i16h(0);
                    build_i16h(0);
                    break;
    }

    uart.send_buf[uart.send_size++] = '\r';
}

int receive_s3_packet(uartstate_t &uart, struct ecudata_t* d, struct fw_data_t &fw_data)
{
    uint8_t descriptor;
    uart.recv_index = 0;
    descriptor = uart.recv_buf[uart.recv_index++];
    uint8_t flags;

    switch(descriptor)
    {
        case SENSOR_DAT:
            d->sens.frequen = recept_i16h();           // averaged RPM
            d->sens.map = recept_i16h();               // averaged MAP pressure
            d->sens.voltage = recept_i16h();           // voltage (avaraged)
            d->sens.temperat = recept_i16h();          // coolant temperature
            d->corr.curr_angle = recept_i16h();        // advance angle
            d->sens.knock_k = recept_i16h();           // knock value
            d->corr.knock_retard = recept_i16h();      // knock retard
            d->airflow = recept_i8h();                 // index of the map axis curve

            flags = recept_i8h();
            //boolean values
            d->ie_valve     = (flags >> 0) & 1;        // IE flag
            d->sens.carb    = (flags >> 1) & 1;        // carb. limit switch flag
            d->sens.gas     = (flags >> 2) & 1;        // gas valve flag
            d->fe_valve     = (flags >> 3) & 1;        // power valve flag
            d->ce_state     = (flags >> 4) & 1;        // CE flag
            d->cool_fan     = (flags >> 5) & 1;        // cooling fan flag
            d->st_block     = (flags >> 6) & 1;        // starter blocking flag
            d->acceleration = (flags >> 7) & 1;        // acceleration enrichment flag
            d->sens.tps = recept_i8h();                // TPS (0...100%, x2)
            d->sens.add_i1 = recept_i16h();            // averaged ADD_I1 voltage
            d->sens.add_i2 = recept_i16h();            // ADD_I2 voltage
            d->ecuerrors_saved_transfer = recept_i16h(); // CE errors
            d->choke_pos = recept_i8h();               // choke position
            d->gasdose_pos = recept_i8h();             // gas dosator position
            d->sens.speed = recept_i16h();             // vehicle speed (2 bytes)
            d->sens.distance = recept_i24h();          // distance (3 bytes)
            d->sens.air_temp = recept_i16h();

            //corrections
            d->corr.strt_aalt = recept_i16h();         // advance angle from start map
            d->corr.idle_aalt = recept_i16h();         // advance angle from idle map
            d->corr.work_aalt = recept_i16h();         // advance angle from work map
            d->corr.temp_aalt = recept_i16h();         // advance angle from coolant temperature correction map
            d->corr.airt_aalt = recept_i16h();         // advance angle from air temperature correction map
            d->corr.idlreg_aac = recept_i16h();        // advance angle correction from idling RPM regulator
            d->corr.octan_aac = recept_i16h();         // octane correction value

            d->corr.lambda = recept_i16h();            // lambda correction
            d->inj_pw = recept_i16h();                 // injector pulse width
            d->sens.tpsdot = recept_i16h();            // TPS opening/closing speed
            break;
        case CHANGEMODE:
            uart.send_mode = uart.recv_buf[uart.recv_index++];
            #ifdef UART_DEBUG
            printf("!CHANGEMODE='%c'\n", uart.send_mode);
            #endif
            break;
        case OP_COMP_NC:
            d->op_actn_code = recept_i16h();
            #ifdef UART_DEBUG
            printf("!OP_COMP_NC=%d\n", d->op_actn_code);
            #endif
            break;
        case EDITAB_PAR:
        {
            uint8_t state = recept_i8h();  //map type
            uint8_t addr = recept_i8h();   //address
            uart.tblIdx = state;
            switch (state)
            {
                case ETMT_STRT_MAP: //start map
                    recept_rb(((uint8_t*)&d->tables_ram.f_str) + addr, F_STR_POINTS); /*F_STR_POINTS max*/
                    break;
                case ETMT_IDLE_MAP: //idle map
                    recept_rb(((uint8_t*)&d->tables_ram.f_idl) + addr, F_IDL_POINTS); /*F_IDL_POINTS max*/
                    break;
                case ETMT_WORK_MAP: //work map
                    recept_rb(((uint8_t*)&d->tables_ram.f_wrk[0][0]) + addr, F_WRK_POINTS_F); /*F_WRK_POINTS_F max*/
                    break;
                case ETMT_TEMP_MAP: //temper. correction map
                    recept_rb(((uint8_t*)&d->tables_ram.f_tmp) + addr, F_TMP_POINTS); /*F_TMP_POINTS max*/
                    break;
                case ETMT_NAME_STR: //name
                    recept_rs((d->tables_ram.name) + addr, F_NAME_SIZE); /*F_NAME_SIZE max*/
                    break;
                case ETMT_VE_MAP:   //VE
                    recept_rb(((uint8_t*)&d->tables_ram.inj_ve[0][0]) + addr, INJ_VE_POINTS_F); /*INJ_VE_POINTS_F max*/
                    break;
                case ETMT_AFR_MAP:  //AFR
                    recept_rb(((uint8_t*)&d->tables_ram.inj_afr[0][0]) + addr, INJ_VE_POINTS_F); /*INJ_VE_POINTS_F max*/
                    break;
                case ETMT_CRNK_MAP: //PW on cranking
                    recept_rw(((uint16_t*)&d->tables_ram.inj_cranking) + addr, INJ_CRANKING_LOOKUP_TABLE_SIZE / 2); /*INJ_CRANKING_LOOKUP_TABLE_SIZE/2 max*/
                    break;
                case ETMT_WRMP_MAP: //Warmup enrichment
                    recept_rb(((uint8_t*)&d->tables_ram.inj_warmup) + addr, INJ_WARMUP_LOOKUP_TABLE_SIZE); /*INJ_WARMUP_LOOKUP_TABLE_SIZE max*/
                    break;
                case ETMT_DEAD_MAP: //Injector dead time
                    recept_rw(((uint16_t*)&d->tables_ram.inj_dead_time) + addr, INJ_DT_LOOKUP_TABLE_SIZE / 4); /*INJ_DT_LOOKUP_TABLE_SIZE/4 max*/
                    break;
                case ETMT_IDLR_MAP: //IAC/PWM position on run
                    recept_rb(((uint8_t*)&d->tables_ram.inj_iac_run_pos) + addr, INJ_IAC_POS_TABLE_SIZE); /*INJ_IAC_POS_TABLE_SIZE max*/
                    break;
                case ETMT_IDLC_MAP: //IAC/PWM position on cranking
                    recept_rb(((uint8_t*)&d->tables_ram.inj_iac_crank_pos) + addr, INJ_IAC_POS_TABLE_SIZE); /*INJ_IAC_POS_TABLE_SIZE max*/
                    break;
                case ETMT_AETPS_MAP: //AE TPS, Note! Here we consider inj_ae_tps_bins and inj_ae_tps_enr as single table
                    recept_rb(((uint8_t*)&d->tables_ram.inj_ae_tps_enr) + addr, INJ_AE_TPS_LOOKUP_TABLE_SIZE * 2); /*INJ_AE_TPS_LOOKUP_TABLE_SIZE*2 max*/
                    break;
                case ETMT_AERPM_MAP: //AE RPM, Note! Here we consider inj_ae_rpm_bins and inj_ae_rpm_enr as single table
                    recept_rb(((uint8_t*)&d->tables_ram.inj_ae_rpm_enr) + addr, INJ_AE_RPM_LOOKUP_TABLE_SIZE * 2); /*INJ_AE_RPM_LOOKUP_TABLE_SIZE*2 max*/
                    break;
                case ETMT_AFTSTR_MAP: //afterstart enrichment map
                    recept_rb(((uint8_t*)&d->tables_ram.inj_aftstr) + addr, INJ_AFTSTR_LOOKUP_TABLE_SIZE); /*INJ_AFTSTR_LOOKUP_TABLE_SIZE max*/
                    break;
                case ETMT_IT_MAP:   //Injection timing
                    recept_rb(((uint8_t*)&d->tables_ram.inj_timing[0][0]) + addr, INJ_VE_POINTS_F); /*INJ_VE_POINTS_F max*/
                    break;
            }
        }
        break;

                case CKPS_PAR:
                    d->param.ckps_edge_type = recept_i4h();
                    d->param.ref_s_edge_type = recept_i4h();
                    d->param.ckps_cogs_btdc = recept_i8h();
                    d->param.ckps_ignit_cogs = recept_i8h();
                    d->param.ckps_engine_cyl = recept_i8h();
                    d->param.merge_ign_outs = recept_i4h();
                    d->param.ckps_cogs_num = recept_i8h();
                    d->param.ckps_miss_num = recept_i8h();
                    d->param.hall_flags = recept_i8h();
                    d->param.hall_wnd_width = recept_i16h();
                    break;

                case STARTR_PAR:
                    d->param.starter_off = recept_i16h();
                    d->param.smap_abandon = recept_i16h();
                    d->param.inj_cranktorun_time = recept_i16h(); //fuel injection
                    d->param.inj_aftstr_strokes = recept_i8h();   //fuel injection
                    d->param.inj_prime_cold = recept_i16h();      //fuel injection
                    d->param.inj_prime_hot = recept_i16h();       //fuel injection
                    d->param.inj_prime_delay = recept_i8h();      //fuel injection
                    break;

                case FUNSET_PAR:
                    d->param.fn_gasoline = recept_i8h();
                    d->param.fn_gas = recept_i8h();
                    d->param.map_lower_pressure = recept_i16h();
                    d->param.map_upper_pressure = recept_i16h();
                    d->param.map_curve_offset = recept_i16h();
                    d->param.map_curve_gradient = recept_i16h();
                    d->param.tps_curve_offset = recept_i16h();
                    d->param.tps_curve_gradient = recept_i16h();
                    d->param.load_src_cfg = recept_i4h();
                    break;

                case INJCTR_PAR:
                    d->param.inj_flags = recept_i8h();
                    d->param.inj_config = recept_i8h();
                    d->param.inj_flow_rate = recept_i16h();
                    d->param.inj_cyl_disp = recept_i16h();
                    d->param.inj_sd_igl_const = recept_i32h();
                    d->param.ckps_engine_cyl = recept_i8h();      //used for calculations on SECU-3 Manager side
                    d->param.inj_timing = recept_i16h();
                    d->param.inj_timing_crk = recept_i16h();
                    break;

                case LAMBDA_PAR:
                    d->param.inj_lambda_str_per_stp = recept_i8h();
                    d->param.inj_lambda_step_size_p = recept_i8h();
                    d->param.inj_lambda_step_size_m = recept_i8h();
                    d->param.inj_lambda_corr_limit_p = recept_i16h();
                    d->param.inj_lambda_corr_limit_m = recept_i16h();
                    d->param.inj_lambda_swt_point = recept_i16h();
                    d->param.inj_lambda_temp_thrd = recept_i16h();
                    d->param.inj_lambda_rpm_thrd = recept_i16h();
                    d->param.inj_lambda_activ_delay = recept_i8h();
                    d->param.inj_lambda_dead_band = recept_i16h();
                    break;

                case RPMGRD_PAR:
                    recept_i8h(); //<--reserved
                    recept_rb((uint8_t*)fw_data.exdata.rpm_grid_points, RPM_GRID_SIZE * sizeof(int16_t));
                    break;

                case FWINFO_DAT:
                    recept_rs(fw_data.fw_signature_info, FW_SIGNATURE_INFO_SIZE);
                    fw_data.cddata.config = recept_i32h();   //<--compile-time options
                    fw_data.cddata.fw_version = recept_i8h(); //<--version of the firmware
                    break;

                default:
                    #ifdef UART_DEBUG
                    printf("!%c\n", descriptor);
                    #endif
                    ;
    }
    return descriptor;
}


