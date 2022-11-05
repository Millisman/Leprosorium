#pragma once
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-attributes"

#include <stdint.h>
#include <stdbool.h>

#define modem stdin

#define MODEM_RX_BUFF_SIZE  512
#define MODEM_TX_BUFF_SIZE  512
#define MODEM_BAUD_RATE     19200

typedef struct __attribute__((__packed__)) {
    char     tx_buff[MODEM_TX_BUFF_SIZE];
    uint16_t tx_buff_head;
    uint16_t tx_buff_tail;
} ModemTxBuff_t;

typedef struct __attribute__((__packed__)) {
    char rx_buff[MODEM_RX_BUFF_SIZE+1];
    uint16_t rx_len;
    uint16_t rx_len_scan;
    bool trip_ok;
    bool trip_err;
    bool trip_prompt;
    bool trip_scan;
    uint8_t save_size;
    char   *save_ptr;
} ModemRxBuff_t;


typedef enum {
    NORMAL_POWER_DOWN,
    UNDER_VOLTAGE_POWER_DOWN,
    UNDER_VOLTAGE_WARNNING,
    OVER_VOLTAGE_POWER_DOWN,
    OVER_VOLTAGE_WARNNING
} SIM800_PowerEvent;

typedef enum {
    CPIN_UNKNOWN,
    CPIN_READY,       //MT is not pending for any password
    CPIN_SIM_PIN,     //MT is waiting SIM PIN to be given
    CPIN_SIM_PUK,     //MT is waiting for SIM PUK to be given
    CPIN_PH_SIM_PIN,  //ME is waiting for phone to SIM card (antitheft)
    CPIN_PH_SIM_PUK,  //ME is waiting for SIM PUK (antitheft)
    CPIN_SIM_PIN2,
    CPIN_SIM_PUK2
} SimCPIN;

typedef enum {
    GSM_NotReg = 0,     // GSM: Not reg
    GSM_RegInHomeNet,   // GSM: Reg in home net
    GSM_NetSearching,   // GSM: Net searching...
    GSM_RegDenied,      // GSM: Reg denied
    GSM_Unknown,        // GSM: Unknown
    GSM_RegInRoaming    // GSM: Reg in Roaming
} SimRegGSM;

// Query Current Connection Status
typedef enum {
    CONN_UNKNOWN,
    CONN_IP_INITIAL,
    CONN_INITIAL,
    CONN_IP_START,
    CONN_IP_CONFIG,
    CONN_IP_GPRSACT,
    CONN_IP_STATUS,
    CONN_TCP_CONNECTING,
    CONN_UDP_CONNECTING,
    CONN_CONNECTING,
    CONN_SERVER_LISTENING,
    CONN_LISTENING,
    CONN_CONNECT_OK,
    CONN_TCP_CLOSING,
    CONN_UDP_CLOSING,
    CONN_CLOSING,
    CONN_TCP_CLOSED,
    CONN_UDP_CLOSED,
    CONN_CLOSED,
    CONN_PDP_DEACT,
    CONN_CONNECTED,
    CONN_IP_PROCESSING,
    CONN_REMOTE_CLOSING,
    CONN_OPENING
} SimConnStatus;

typedef enum {
    NotPressed,
    Pressed,
    Booting
} PowerKey;

typedef enum {
    UNKNOWN,
    DISABLED,
    AT_OK,
    ACTIVATING,
    GPRS_CONNECTED
} ModemStatus;

typedef struct __attribute__((__packed__)) {
    char ATI[24];
    char s_GCAP[24];
    char s_GMI[24];
    char s_GMM[24];
    char s_GMR[32];
    char s_GOI[24];
    char s_GSN[20];
    char s_GSV[20];
    char s_CIMI[20];
    char s_IP[16];
} gsm_init_info_t;





typedef void (*GsmPowerONOFF)(bool on);

typedef struct __attribute__((__packed__)) {
    gsm_init_info_t info;
//     bool m_Debug;
    bool        scheduled_scan;
//     uint32_t    scheduled_ts_ms;
    GsmPowerONOFF   Gsm_Power;
//     GsmRecvHook on_Recv;
//     bool flag_debug;
//     char *found_in;
//     bool m_Busy;
//     uint16_t sched_size;
//     bool qs_;
    PowerKey button;
    ModemStatus mode;
//     uint8_t istage;
//     uint8_t faults;
    bool observe_GSM_OK;
    bool observe_GSM_ERROR;
    bool observe_GSM_PROMPT;
    bool observe_GSM_DATA_ACCEPT;
// //     uint32_t moment;
// //     uint32_t nov_ms;
    uint8_t gsm_mode;
    SimConnStatus ip_state;
//     SimCPIN cpin_Event;
    SimRegGSM netreg_Event;
//     SimConnStatus connState_Event;


    uint8_t counter_100ms;

    uint8_t boot_level;


} SIM800_Man;





void _hal_modem_init_port();
void _hal_modem_write(const uint8_t c);
void gsm_proc_power_pin(bool en);


void gsm_proc_begin(SIM800_Man *man);
void gsm_proc_update(SIM800_Man *man, ModemRxBuff_t *rx);
void gsm_proc_update_100ms(SIM800_Man *man, ModemRxBuff_t *rx);
void gsm_proc_update_1s(SIM800_Man *man, ModemRxBuff_t *rx);

void gsm_proc_deactivate(SIM800_Man *man);

typedef struct __attribute__((__packed__)) {
    const char   *str;
    SimConnStatus ip_status;
} gsm_ipstates_t;


typedef struct __attribute__((__packed__)) {
    const char *cmd;
    void (*func)(char *, SIM800_Man *);
} gsm_reply_t;



typedef struct __attribute__((__packed__)) {
    const char *cmd;
    char *result;
    uint8_t     size;
} gsm_first_init_t;



// static char info_I[20]; //", ati_i, sizeof(ati_i)},
// static char info_GCAP[32];//", ati_i, sizeof(ati_i)},
// static char info_GMI[32];//", ati_i, sizeof(ati_i)},
// static char info_GMM[32];//", ati_i, sizeof(ati_i)},
// static char info_GMR[32];//", ati_i, sizeof(ati_i)},
// static char info_GOI[32];//", ati_i, sizeof(ati_i)},
// static char info_GSN[32];//", ati_i, sizeof(ati_i)},





