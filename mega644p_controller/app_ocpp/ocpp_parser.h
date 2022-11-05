#pragma once

#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-attributes"
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"



#include "mcu/mcu.h"
#include "ocpp_strings.h"
#include "uuid.h"
#include "mjson.h"
#include <stdbool.h>
#include <stdio.h>
#include "rtclib.h"

#define  LOG_ON

typedef struct __attribute__((__packed__)) {
    char KeyStart[30];
    char Key[25];
} ws_key_struct;


typedef struct __attribute__((__packed__)) {
    const char *key;
    const char *value;
} cons_string_pair;

enum Send_Frame_Enum {Send_Frame_Code, Send_Frame_Action, Send_Frame_Content, Send_Frame_SEND};


typedef enum {
    STR_PGM,
    STR_RAM,
    INT32
} ValType_t;

// /usr/lib64/gcc/avr/7/ld: obj/libapp_ocpp.a(ocpp_strings.o):(.rodata.OCPP_AuthStaPair+0x0): multiple definition of `OCPP_AuthStaPair'; obj/libapp_ocpp.a(ocpp_man.o):(.rodata.OCPP_AuthStaPair+0x0): first defined here

// typedef struct {
//     const char   *val_chargePointVendor;
//     const char   *val_chargePointModel;
//     const char   *val_chargePointSerialNumber;
//     const char   *val_firmwareVersion;
//     //---------
//     const char   *val_chargeBoxSerialNumber;
//     const char   *val_iccid;
//     const char   *val_imsi;
//     const char   *val_meterSerialNumber;
//     const char   *val_meterType;
// } ocpp_def_str_t;

// // // // const char pval_chargePointVendor[] PROGMEM = "Millisman";
// // // // const char pval_chargePointModel[]  PROGMEM = "TestStation";
// // // // const char pval_chargePointSerialNumber[]  PROGMEM = "1000";
// // // // const char pval_firmwareVersion[] PROGMEM = "apha 0.1.1";
// // // // //---------
// // // // const char pval_chargeBoxSerialNumber[] PROGMEM = "777";
// // // // const char pval_iccid[] PROGMEM = ">iccid<";
// // // // const char pval_imsi[] PROGMEM = ">imsi<";
// // // // const char pval_meterSerialNumber[] PROGMEM = "001";
// // // // const char pval_meterType[] PROGMEM = "SPI_ADE7758";



typedef struct __attribute__((__packed__)) {
    const char *key;
    const char *value;
} str_Key_Value_t;

typedef enum {
    NONE_STATE,             Http_Connection_Upgrade, 

    Authorize,              BootNotification,               Heartbeat,
    MeterValues,            SetChargingProfile,             StatusNotification,
    StartTransaction,       StopTransaction,                TriggerMessage,
    RemoteStartTransaction, RemoteStopTransaction,          ChangeConfiguration,
    GetConfiguration,       Reset, UpdateFirmware,          FirmwareStatusNotification,
    GetDiagnostics,         DiagnosticsStatusNotification,  UnlockConnector,
    ClearChargingProfile,   ChangeAvailability,
    // ----------------
    CancelReservation,      ClearCache,             DataTransfer,
    GetLocalListVersion,    GetCompositeSchedule,   ReserveNow,
    SendLocalList,
    // ----------------
    OCPP_STATE_MIN = Authorize, OCPP_STATE_MAX = SendLocalList
} OCPP_States;

typedef struct __attribute__((__packed__)) {
    OCPP_States state;
    const char *label;
} OCPP_State_pair;

// const OCPP_State_pair OCPP_sp[] = {
//     { Authorize,            str_Authorize },
//     { BootNotification,     str_BootNotification },
//     { Heartbeat,            str_Heartbeat },
//     { MeterValues,          str_MeterValues },
//     { SetChargingProfile,   str_SetChargingProfile },
//     { StatusNotification,   str_StatusNotification },
//     { StartTransaction,     str_StartTransaction },
//     { StopTransaction,      str_StopTransaction },
//     { TriggerMessage,       str_TriggerMessage },
//     { RemoteStartTransaction,           str_RemoteStartTransaction },
//     { RemoteStopTransaction,            str_RemoteStopTransaction },
//     { ChangeConfiguration,              str_ChangeConfiguration },
//     { GetConfiguration,                 str_GetConfiguration },
//     { Reset,                            str_Reset },
//     { UpdateFirmware,                   str_UpdateFirmware },
//     { FirmwareStatusNotification,       str_FirmwareStatusNotification },
//     { GetDiagnostics,                   str_GetDiagnostics },
//     { DiagnosticsStatusNotification,    str_DiagnosticsStatusNotification },
//     { UnlockConnector,      str_UnlockConnector },
//     { ClearChargingProfile, str_ClearChargingProfile },
//     { ChangeAvailability,   str_ChangeAvailability },
//     // -------------------------------------------
//     { CancelReservation,    str_CancelReservation },
//     { ClearCache,           str_ClearCache },
//     { DataTransfer,         str_DataTransfer },
//     { GetLocalListVersion,  str_GetLocalListVersion },
//     { GetCompositeSchedule, str_GetCompositeSchedule },
//     { ReserveNow,           str_ReserveNow },
//     { SendLocalList,        str_SendLocalList },
//     {NONE_STATE, 0}
// };

// const char str_OCPP_States_NONE[] PROGMEM = "NONE";

void print_OCPP_States(const OCPP_States state);

// void print_OCPP_States(const OCPP_States state) {
//     const char *l = str_OCPP_States_NONE;
//     for (const OCPP_State_pair* f = OCPP_sp; f->label; f++) {
//         if (f->state == state) { l = f->label; }
//     }
//     printf_P(PSTR("OCPP State ["));
//     printf_P(l);
//     printf("]\n");
// }




typedef enum {
    RegistrationStatus_UNKNOWN,
    RegistrationStatus_Accepted,
    RegistrationStatus_Pending,
    RegistrationStatus_Rejected
} OCPP_RegistrationStatus;

typedef struct __attribute__((__packed__)) {
    const OCPP_RegistrationStatus state;
    const char *label;
} OCPP_RegistrationStatus_pair;



typedef enum { 
    AuthorizationStatus_UNKNOWN,
    AuthorizationStatus_Accepted,
    AuthorizationStatus_Blocked,
    AuthorizationStatus_Expired,
    AuthorizationStatus_Invalid,
    AuthorizationStatus_ConcurrentTx
} OCPP_AuthorizationStatus;

typedef struct __attribute__((__packed__)) {
    const OCPP_AuthorizationStatus state;
    const char *label;
} OCPP_AuthorizationStatus_pair;





#define in_range(c, lo, up)  ((uint8_t)c >= lo && (uint8_t)c <= up)
#define isdigit(c)           in_range(c, '0', '9')






typedef struct __attribute__((__packed__)) {
    bool    connected;
    bool    shed_conf_msg;
    FILE    *cout;
    ws_key_struct ws_key;


    char req__uuid_36[37];
    char conf_uuid_36[37];

    rtc_t rtc;


    uint16_t                uuid_n;
    uuid_bytes_t            uuid_b;
    struct uuid_RFC4122     uuid_s;
    // ------------------------------
    char    *message_Code_ptr;
    uint8_t  message_Code_len;
    char    *message_uuid_ptr;
    uint8_t  message_uuid_len;
    char    *message_request_ptr;
    uint8_t  message_request_len;
    char    *message_jsonStr_ptr;
    uint16_t message_jsonStr_len;
    OCPP_States State;

    char    *json_payload;
    uint16_t json_payload_length;

    cons_string_pair tmp_str_pair;

    char str_processing_uuid[37];

    OCPP_AuthorizationStatus    Authorize_AuthorizationStatus;
    char                        Authorize_expiryDate[28];

    uint32_t                BootNotification_interval;
    OCPP_RegistrationStatus BootNotification_RegistrationStatus; // TODO UNKNOWN -def

//     int tmp_int0, tmp_int1;
//     const char *tmp_char0, *tmp_char1;
//     double m_BufDouble;


    uint32_t UnixTime;
    uint32_t TransactionId;
//     const char *  _json_ptr;
//     int _json_len; // int!
//     int32_t __TransactionId;
} ocpp_structure_t;



    // {"currentTime":"2022-02-19T03:16:16.633Z"}
bool        OCPP_Parser_load_System_UnixTime(ocpp_structure_t *ocpp);

uint16_t    OCPP_write_req(ocpp_structure_t *ocpp, const bool real);
// void        OCPP_Conf_Begin(ocpp_structure_t *ocpp);
void        OCPP_Parser_processing_conf(ocpp_structure_t *ocpp);
void        OCPP_Parser_processing_error(ocpp_structure_t *ocpp);
