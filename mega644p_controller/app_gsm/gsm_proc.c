#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>

#include "gsm_proc.h"
#include "mcu/mcu.h"
#include "mcu/uart.h"
#include "utils/cmd.h"
#include <string.h>

void gsm_proc_begin(SIM800_Man *man) {
    man->Gsm_Power(true);
    man->button                  = Pressed;
    man->counter_100ms           = 17; // pressing time >1500 ms
    man->mode                  = UNKNOWN;
    man->observe_GSM_OK          = false;
    man->observe_GSM_ERROR       = false;
    man->observe_GSM_PROMPT      = false;
    man->observe_GSM_DATA_ACCEPT = false;
    man->scheduled_scan          = false;
//     man->cpin_Event              = CPIN_UNKNOWN;
    man->netreg_Event            = GSM_Unknown;
    man->ip_state                = CONN_UNKNOWN;
    man->boot_level              = 0;
}

const char PSTR_AT[]  PROGMEM = "AT";
const char PSTR_EOL[] PROGMEM = "\r\n";

void modem_at_P(const char *c) {
    fprintf_P(modem, PSTR_AT);
    if (c) { fprintf_P(modem, c); }
    fprintf_P(modem, PSTR_EOL);
}

void modem_strace_P(const char *name, const char *str) {
    if (name && str) {
        printf_P(PSTR_AT);
        printf_P(name);
        printf_P(PSTR("\t[%s]\n"), str);
    }
}

const char PSTR_NORMAL_POWER_DOWN[]         PROGMEM = "NORMAL POWER DOWN";
const char PSTR_UNDER_VOLTAGE_POWER_DOWN[]  PROGMEM = "UNDER-VOLTAGE POWER DOWN";
const char PSTR_UNDER_VOLTAGE_WARNNING[]    PROGMEM = "UNDER-VOLTAGE WARNNING";
const char PSTR_OVER_VOLTAGE_POWER_DOWN[]   PROGMEM = "OVER-VOLTAGE POWER DOWN";
const char PSTR_OVER_VOLTAGE_WARNNING[]     PROGMEM = "OVER-VOLTAGE WARNNING";

const char PSTR_CALL_READY[]    PROGMEM = "Call Ready";
const char PSTR_SMS_READY[]     PROGMEM = "SMS Ready";






const char PSTR_STATE[]             PROGMEM = "STATE";
const char PSTR_CONNECT_OK[]        PROGMEM ="CONNECT OK";
const char PSTR_CONNECTED[]         PROGMEM ="CONNECTED";
const char PSTR_IP_INITIAL[]        PROGMEM ="IP INITIAL";
const char PSTR_IP_START[]          PROGMEM ="IP START";
const char PSTR_IP_GPRSACT[]        PROGMEM ="IP GPRSACT";
const char PSTR_IP_STATUS[]         PROGMEM ="IP STATUS";
const char PSTR_INITIAL[]           PROGMEM ="INITIAL";
const char PSTR_REMOTE_CLOSING[]    PROGMEM ="REMOTE CLOSING";
const char PSTR_TCP_CLOSED[]        PROGMEM ="TCP CLOSED";
const char PSTR_UDP_CLOSED[]        PROGMEM ="UDP CLOSED";
const char PSTR_CLOSED[]            PROGMEM ="CLOSED";
const char PSTR_PDP_DEACT[]         PROGMEM ="PDP DEACT";
const char PSTR_TCP_CONNECTING[]    PROGMEM ="TCP CONNECTING";
const char PSTR_UDP_CONNECTING[]    PROGMEM ="UDP CONNECTING";
const char PSTR_CONNECTING[]        PROGMEM ="CONNECTING";
#if 0
const char PSTR_IP_PROCESSING[]     PROGMEM ="IP PROCESSING";
const char PSTR_IP_CONFIG[]         PROGMEM ="IP CONFIG";
const char PSTR_TCP_CLOSING[]       PROGMEM ="TCP CLOSING";
const char PSTR_UDP_CLOSING[]       PROGMEM ="UDP CLOSING";
const char PSTR_CLOSING[]           PROGMEM ="CLOSING";
const char PSTR_SERVER_LISTENING[]  PROGMEM ="SERVER LISTENING";
const char PSTR_LISTENING[]         PROGMEM ="LISTENING";
const char PSTR_OPENING[]           PROGMEM ="OPENING";

#endif

static gsm_ipstates_t gsm_ip_state[] = {
    { PSTR_INITIAL,             CONN_INITIAL},
    { PSTR_IP_START,            CONN_IP_START},
    { PSTR_IP_STATUS,           CONN_IP_STATUS},
    { PSTR_IP_GPRSACT,          CONN_IP_GPRSACT},
    { PSTR_TCP_CLOSED,          CONN_TCP_CLOSED},
    { PSTR_UDP_CLOSED,          CONN_UDP_CLOSED},
    { PSTR_CONNECT_OK,          CONN_CONNECT_OK},
    { PSTR_REMOTE_CLOSING,      CONN_REMOTE_CLOSING},
    { PSTR_CLOSED,              CONN_CLOSED},
    { PSTR_CONNECTED,           CONN_CONNECTED},
    { PSTR_IP_INITIAL,          CONN_IP_INITIAL},
    { PSTR_PDP_DEACT,           CONN_PDP_DEACT},
    { PSTR_TCP_CONNECTING,      CONN_TCP_CONNECTING},
    { PSTR_UDP_CONNECTING,      CONN_UDP_CONNECTING},
    { PSTR_CONNECTING,          CONN_CONNECTING},
#if 0
    { PSTR_IP_CONFIG,           CONN_IP_CONFIG},
    { PSTR_IP_PROCESSING,       CONN_IP_PROCESSING},
    { PSTR_TCP_CLOSING,         CONN_TCP_CLOSING},
    { PSTR_UDP_CLOSING,         CONN_UDP_CLOSING},
    { PSTR_CLOSING,             CONN_CLOSING},
    { PSTR_SERVER_LISTENING,    CONN_SERVER_LISTENING},
    { PSTR_LISTENING,           CONN_LISTENING},
    { PSTR_OPENING,             CONN_OPENING},

#endif
    {0, 0}
};

void gsm_handler_STATE(char *c, SIM800_Man *man) {
    char *d = getFirstArg(c);
    printf_P(PSTR_STATE);
    for(gsm_ipstates_t* f = gsm_ip_state; f->str; f++) {
        if (strstr_P(d, f->str) != NULL) {
            man->ip_state = f->ip_status;
            printf_P(PSTR(":\"%s\",%u\n"), d, man->ip_state);
            return;
        }
    }
    printf_P(PSTR("!:\"%s\"\n"), d);
}

void gsm_handler_waste(char *c, SIM800_Man *d) { printf_P(PSTR("GSM waste [%s]\n"), c); }

static gsm_reply_t gsm_procs[] = {
    {PSTR_CALL_READY,               gsm_handler_waste},
    {PSTR_SMS_READY,                gsm_handler_waste},
    {PSTR_NORMAL_POWER_DOWN,        gsm_handler_waste},
    {PSTR_UNDER_VOLTAGE_POWER_DOWN, gsm_handler_waste},
    {PSTR_UNDER_VOLTAGE_WARNNING,   gsm_handler_waste},
    {PSTR_OVER_VOLTAGE_POWER_DOWN,  gsm_handler_waste},
    {PSTR_OVER_VOLTAGE_WARNNING,    gsm_handler_waste},
    {PSTR_STATE,                    gsm_handler_STATE},
    {0, 0}
};

void gsm_proc_update(SIM800_Man *man, ModemRxBuff_t *rx) {

    if (rx->trip_ok) {
        rx->trip_ok = false;
        man->observe_GSM_OK = true;
        printf_P(PSTR("OK\n"));
    }

    if (rx->trip_err) {
        rx->trip_err = false;
        man->observe_GSM_ERROR = true;
        printf_P(PSTR("ERROR\n"));

    }

    if (rx->trip_prompt) {
        rx->trip_prompt = false;
        man->observe_GSM_PROMPT = true;

        printf_P(PSTR("PROMPT\n"));
    }

    if (rx->trip_scan) {
        rx->trip_scan = false;
#if 1
        printf_P(PSTR("scan %u ["), rx->rx_len_scan);
        for (uint8_t a = 0; a < rx->rx_len_scan; ++a) {
            uart0_putc(rx->rx_buff[a]);
        }
        uart0_puts("]\n");
#endif
        for(gsm_reply_t* f = gsm_procs; f->cmd; f++) {
            if (strstr_P((char *)&rx->rx_buff, f->cmd) != NULL) {
                // f->func(getFirstArg((char *)&rx->rx_buff));
                f->func((char *)&rx->rx_buff, man);
                return;
            }
        }

        if (*&rx->rx_buff) { printf_P(PSTR("GSM? [%s]\n"), &rx->rx_buff); }

    }
}



const char PSTR_E0[]  PROGMEM = "E0";       // ATE0 Echo off
const char PSTR_I[]   PROGMEM = "I";        // ATI
const char PSTR_pGCAP[]  PROGMEM = "+GCAP";    // Request complete TA capabilities list
const char PSTR_pGMI[]  PROGMEM = "+GMI";     // Request manufacturer identification
const char PSTR_pGMM[]  PROGMEM = "+GMM";     // Request TA model identification
const char PSTR_pGMR[]  PROGMEM = "+GMR";     // Request TA revision identification of software release
const char PSTR_pGOI[]  PROGMEM = "+GOI";     // Request global object identification
const char PSTR_pGSN[]  PROGMEM = "+GSN";     // Request TA serial number identification (IMEI)
const char PSTR_pCPINq[]  PROGMEM = "+CPIN?";
const char PSTR_pCREGq[]  PROGMEM = "+CREG?";
const char PSTR_pGSV[]  PROGMEM = "+GSV";
const char PSTR_pCIMI[]  PROGMEM = "+CIMI";






void gsm_proc_update_100ms(SIM800_Man *man, ModemRxBuff_t *rx) {

    if (man->counter_100ms) { --man->counter_100ms; }

    switch (man->button) {

        case Pressed:
            if (!man->counter_100ms) {
                man->Gsm_Power(false);
                man->button = Booting;
                man->counter_100ms = 50;
            }
            break;

        case Booting:
            if (!man->counter_100ms) {
                man->button = NotPressed;
                man->counter_100ms = 40;
            }
            break;

        case NotPressed:

            switch (man->mode ) {

                case DISABLED:
                    break;

                case UNKNOWN:

                    if (!man->counter_100ms) {
                        gsm_proc_begin(man);
                    } else if (man->observe_GSM_OK) {
                        man->observe_GSM_OK = false;
                        man->mode = AT_OK;
                    } else { modem_at_P(0); }
                    break;

                case AT_OK: // found - init process

                    if (!man->counter_100ms || man->observe_GSM_OK) {
                        man->counter_100ms = 10;
                        if (man->observe_GSM_OK) ++man->boot_level;

                        switch (man->boot_level) {

                            case 0:
                                modem_at_P(PSTR_E0); // ATE0 Echo off
                            break;

                            case 1:
                                modem_at_P(PSTR_I); // ATI
                                rx->save_size = sizeof(man->info.ATI);
                                rx->save_ptr  = man->info.ATI;
                                break;

                            case 2:
                                modem_strace_P(PSTR_I, man->info.ATI);
                                modem_at_P(PSTR_pGCAP); // Request complete TA capabilities list
                                rx->save_size = sizeof(man->info.s_GCAP);
                                rx->save_ptr  = man->info.s_GCAP;
                                break;

                            case 3:
                                modem_strace_P(PSTR_pGCAP, man->info.s_GCAP);
                                modem_at_P(PSTR_pGMI); // Request manufacturer identification
                                rx->save_size = sizeof(man->info.s_GMI);
                                rx->save_ptr  = man->info.s_GMI;
                                break;

                            case 4:
                                modem_strace_P(PSTR_pGMI, man->info.s_GMI);
                                modem_at_P(PSTR_pGMM); // Request TA model identification
                                rx->save_size = sizeof(man->info.s_GMM);
                                rx->save_ptr  = man->info.s_GMM;
                                break;

                            case 5:
                                modem_strace_P(PSTR_pGMM, man->info.s_GMM);
                                modem_at_P(PSTR_pGMR); // Request TA revision identification of software release
                                rx->save_size = sizeof(man->info.s_GMR);
                                rx->save_ptr  = man->info.s_GMR;
                                break;

                            case 6:
                                modem_strace_P(PSTR_pGMR, man->info.s_GMR);
                                modem_at_P(PSTR_pGOI); // Request global object identification
                                rx->save_size = sizeof(man->info.s_GOI);
                                rx->save_ptr  = man->info.s_GOI;
                                break;

                            case 7:
                                modem_strace_P(PSTR_pGOI, man->info.s_GOI);
                                modem_at_P(PSTR_pGSN); // Request TA serial number identification (IMEI)
                                rx->save_size = sizeof(man->info.s_GSN);
                                rx->save_ptr  = man->info.s_GSN;
                                break;

                            case 8:
                                modem_strace_P(PSTR_pGSN, man->info.s_GSN);
                                modem_at_P(PSTR_pCPINq);
                                break;

                            case 9:
                                modem_at_P(PSTR_pCREGq);
                                break;

                            case 10:
                                modem_at_P(PSTR_pGSV);
                                rx->save_size = sizeof(man->info.s_GSV);
                                rx->save_ptr  = man->info.s_GSV;
                                break;

                            case 11:
                                modem_strace_P(PSTR_pGSV, man->info.s_GSV);
                                modem_at_P(PSTR_pCIMI);
                                rx->save_size = sizeof(man->info.s_CIMI);
                                rx->save_ptr  = man->info.s_CIMI;
                                break;

                            case 12:
                                modem_strace_P(PSTR_pCIMI, man->info.s_CIMI);
                                man->mode = ACTIVATING;
                                man->observe_GSM_OK = false;
                            break;


                            default:
                                printf_P(PSTR("boot level = %u\n"), man->boot_level);

                        }
                        man->observe_GSM_OK = false;

                    }
                    break;

                case ACTIVATING:

                    if (!man->counter_100ms || man->observe_GSM_OK) {
                        man->counter_100ms = 10;
//                         if (man->observe_GSM_OK) ++man->boot_level;

                        switch (man->ip_state) {

                            case CONN_IP_INITIAL: // not actual
                            case CONN_INITIAL:
                                if (man->observe_GSM_OK) {
                                    modem_at_P(PSTR("+CSTT=\"internet\",\"gprs\",\"gprs\""));
                                } else {
                                    modem_at_P(PSTR("+CGATT=1"));
                                }
                                break;

                            case CONN_IP_START:
                                printf_P(PSTR("CONN_IP_START"));
                                modem_at_P(PSTR("+CIICR")); // 85sec max , traffic calculate!!

                                break;

                            case CONN_IP_GPRSACT:
                                printf_P(PSTR("CONN_IP_GPRSACT"));
                                //  Get Local IP Address (AT+CIFSR ---> 100.91.34.225)
                                modem_at_P(PSTR("+CIFSR"));
                                rx->save_size = sizeof(man->info.s_IP);
                                rx->save_ptr  = man->info.s_IP;
                                break;

                            case CONN_IP_STATUS:
                                modem_strace_P(PSTR("+CIFSR"), man->info.s_IP);
                                printf_P(PSTR("CONN_IP_STATUS\n"));


                                modem_at_P(PSTR("+CIPSSL=1"));
                                modem_at_P(PSTR("+CIPQSEND=1")); // fast send
                                modem_at_P(PSTR("+CIPHEAD=1")); // add prefix +IPD,<SIZE>:
//                                 AT+CIPSSL=1
                                modem_at_P(PSTR("+CIPSTART=\"TCP\",\"solar-track.me\",\"543\""));


// //                                 AT+CIPSTART="TCP","116.228.221.51","8500"


                                man->ip_state = 99;

//                                 modem_at_P(PSTR("+CIPQSEND=1")); // fast send
//                                 modem_at_P(PSTR("+CIPHEAD=1")); // add prefix +IPD,<SIZE>:
//                                 modem_at_P(PSTR("+CIPSTART=\"TCP\",\"172.241.27.173\",8080"));
                                // scan 16 [ALREADY CONNECT]
                                // GSM? [ALREADY CONNECT]
                                // scan 11 [CONNECT OK]
                                // GSM? [CONNECT OK]
                                break;

                            case CONN_CONNECTED: // not actual
                            case CONN_CONNECT_OK: // << -- WORK STATE
                                // WORK STATE
                                printf_P(PSTR("CONNECTED!\n"));
                                break;

                            case CONN_CLOSED: // not actual
                            case CONN_REMOTE_CLOSING: // not actual
                            case CONN_TCP_CLOSED:
                            case CONN_UDP_CLOSED:
                                // scan 7 [CLOSED]
                                gsm_proc_deactivate(man);
                                break;
                            case CONN_TCP_CONNECTING:
                            case CONN_UDP_CONNECTING: // << -- wait
                                printf_P(PSTR("Wait SSL...\n"));
                                break;


                            case CONN_PDP_DEACT:
                                gsm_proc_deactivate(man);
                                // DEACT - restart
                                // scan 8 [SHUT OK]
                                // GSM? [SHUT OK]
                                break;

//                             default: {
//                                 printf_P(PSTR("ACTIVATING ip_state? %u\n"), man->ip_state);
//                             }
                        }
                    }

                    man->observe_GSM_OK = false;
                break;

                case GPRS_CONNECTED: // conncted
                    printf_P(PSTR("gsm conncted\n"));



                    break;
            }
    }

}


void gsm_proc_deactivate(SIM800_Man *man) {
    modem_at_P(PSTR("+CIPSHUT"));
    man->ip_state = CONN_UNKNOWN;
}




void gsm_proc_update_1s(SIM800_Man *man, ModemRxBuff_t *rx) {
    if (man->boot_level > 10 && man->mode == ACTIVATING) {
        modem_at_P(PSTR("+CIPSTATUS"));
    }
    //     printf("info_I    {%s}\n", man->iinfo.info_I);
    //     printf("info_GCAP {%s}\n", man->iinfo.info_GCAP);
    //     printf("info_GMI  {%s}\n", man->iinfo.info_GMI);
    //     printf("info_GMM  {%s}\n", man->iinfo.info_GMM);
    //     printf("info_GMR  {%s}\n", man->iinfo.info_GMR);
    //     printf("info_GOI  {%s}\n", man->iinfo.info_GOI);
    //     printf("info_GSN  {%s}\n", man->iinfo.info_GSN);
}
