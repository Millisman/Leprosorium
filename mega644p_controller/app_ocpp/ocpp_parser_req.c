#include "ocpp_parser.h"
#include "ocpp_strings.h"
#include "Base64.h"
#include "sha1.h"
#include "uuid.h"

#define LOG_ON

#ifdef LOG_ON
#define LOGGING(x) x
#else
#define LOGGING(x)
#endif



const char        str_chargePointVendor[] PROGMEM = "chargePointVendor";
const char         str_chargePointModel[] PROGMEM = "chargePointModel";
const char  str_chargePointSerialNumber[] PROGMEM = "chargePointSerialNumber";
const char    str_chargeBoxSerialNumber[] PROGMEM = "chargeBoxSerialNumber";
const char          str_firmwareVersion[] PROGMEM = "firmwareVersion";
const char                    str_iccid[] PROGMEM = "iccid";
const char                     str_imsi[] PROGMEM = "imsi";
const char        str_meterSerialNumber[] PROGMEM = "meterSerialNumber";
const char                str_meterType[] PROGMEM = "meterType";

const char pval_chargePointVendor[] PROGMEM = "Millisman";
const char pval_chargePointModel[]  PROGMEM = "TestStation";
const char pval_chargePointSerialNumber[]  PROGMEM = "1000";
const char pval_firmwareVersion[] PROGMEM = "apha 0.1.1";
//---------
const char pval_chargeBoxSerialNumber[] PROGMEM = "777";
const char pval_iccid[] PROGMEM = ">iccid<";
const char pval_imsi[] PROGMEM = ">imsi<";
const char pval_meterSerialNumber[] PROGMEM = "001";
const char pval_meterType[] PROGMEM = "SPI_ADE7758";





const str_Key_Value_t Pairs_BootNotification[] = {
    { str_chargePointVendor,        pval_chargePointVendor },
    { str_chargePointModel,         pval_chargePointModel },
    { str_chargePointSerialNumber,  pval_chargePointSerialNumber },
    { str_firmwareVersion,          pval_firmwareVersion },
    { str_chargeBoxSerialNumber,    pval_chargeBoxSerialNumber },
    { str_iccid,                    pval_iccid },
    { str_imsi,                     pval_imsi },
    { str_meterSerialNumber,        pval_meterSerialNumber },
    { str_meterType,                pval_meterType },
    {0, 0}
};


void __write_ocpp_quotation(FILE *f) {
    fprintf_P(f, PSTR("\""));
}

void __write_ocpp_str_P(FILE *f, const char *key) { // Key in PGM
    __write_ocpp_quotation(f);
    fprintf_P(f, key);
    __write_ocpp_quotation(f);
}

void __write_ocpp_Key_Value(FILE *f, const str_Key_Value_t *sp, const bool Ram_Val) {
    while (sp->key && sp->value) {
        __write_ocpp_str_P(f, sp->key);
        fprintf_P(f, PSTR(":"));
        if (Ram_Val) {
            fprintf_P(f, PSTR("\"%s\""), sp->value);
        } else {
            __write_ocpp_str_P(f, sp->value);
        }
        sp++;
        if (sp->key && sp->value) { fprintf_P(f, PSTR(",")); }
    }
}

void __write_ocpp_Key_Value_i32(FILE *f, const char *PGM_key, const int32_t val) {
    if (PGM_key) {
        __write_ocpp_str_P(f, PGM_key);
        fprintf_P(f, PSTR(":"));
        fprintf_P(f, PSTR("%ld"), val);
    }
}

void __write_Json_start_P(FILE *f, const char *str) {
    fprintf_P(f, str);
    fprintf_P(f, PSTR("\",{"));
}


//        2022-02-19T03:16:16.000Z
// "key":"2022-02-19T03:16:16.000Z"

void __write_Time_to_ocpp(ocpp_structure_t *ocpp, const char *key) {
    if (key) {
        __write_ocpp_str_P(ocpp->cout, key);
        fprintf_P(ocpp->cout, PSTR(":"));
        __write_ocpp_quotation(ocpp->cout);
    }
    rtc_t rtc;
    TimeFromUnix(&ocpp->UnixTime, &rtc);
    fprintf_P(ocpp->cout, PSTR("20%02u-%02u-%02uT%02u:%02u:%02u.000Z"),
              rtc.yOff, rtc.m, rtc.d, rtc.hh, rtc.mm, rtc.ss );
    if (key) { __write_ocpp_quotation(ocpp->cout); }
}

uint16_t OCPP_write_req(ocpp_structure_t *ocpp, const bool real) {
    (void)real;

//     if (print) { // regen
        __uuid_generate_random(ocpp->uuid_b, &ocpp->uuid_n); //calling function to generate GUID
        uuid_unpack(ocpp->uuid_b, &ocpp->uuid_s);
        // UUID = 36 B

    // 00000000-0000-0000-0309-000000000000
    fprintf_P(ocpp->cout, PSTR("[2,\"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\",\""), // ALERT
              ocpp->uuid_s.time_low,
              ocpp->uuid_s.time_mid,
              ocpp->uuid_s.time_hi_and_version,
              (uint8_t)(ocpp->uuid_s.clock_seq >> 8),
              (uint8_t)(ocpp->uuid_s.clock_seq & 0xFF),
              ocpp->uuid_s.node[0],
              ocpp->uuid_s.node[1],
              ocpp->uuid_s.node[2],
              ocpp->uuid_s.node[3],
              ocpp->uuid_s.node[4],
              ocpp->uuid_s.node[5]
    );

//     ms << PGM << PSTR("\",\"");

    str_Key_Value_t KeyValue[3] = {{0}};

    switch (ocpp->State) {

        case Authorize:
            __write_Json_start_P(ocpp->cout, str_Authorize);

            KeyValue[0].key   = str_idTag;    // PGM
            KeyValue[0].value = "B4A63CDF";   // RAM
            __write_ocpp_Key_Value(ocpp->cout, KeyValue, true);   // RAM value!

        break;

        case BootNotification:
            __write_Json_start_P(ocpp->cout, str_BootNotification);
            __write_ocpp_Key_Value(ocpp->cout, Pairs_BootNotification, false);
        break;

        case Heartbeat: // Charge points will send heartbeats roughly every 3 minutes, CMS return time
            __write_Json_start_P(ocpp->cout, str_Heartbeat);
            // no __write_ocpp_Key_Value
        break;

        //"Available" "Preparing" "Charging" "SuspendedEVSE" "SuspendedEV"
        //"Finishing" "Reserved" "Unavailable" "Faulted" "NOT_SET"
        case StatusNotification: // reply [3,"44de7ca5-894e-4759-9351-adac869580ec",{}]
            __write_Json_start_P(ocpp->cout, str_StatusNotification);
            __write_ocpp_Key_Value_i32(ocpp->cout, str_connectorId, 1);
            fprintf_P(ocpp->cout, PSTR(","));
            KeyValue[0].key   = str_errorCode;  // PGM
            KeyValue[0].value = str_NoError;    // PGM
            KeyValue[1].key   = str_status;     // PGM
            KeyValue[1].value = str_Available;  // PGM
            __write_ocpp_Key_Value(ocpp->cout, KeyValue, false);    // PGM value!
        break;

        case StartTransaction:
            __write_Json_start_P(ocpp->cout, str_StartTransaction);
            __write_ocpp_Key_Value_i32(ocpp->cout, str_connectorId, 1);
            fprintf_P(ocpp->cout, PSTR(","));

            KeyValue[0].key   = str_idTag;    // PGM
            KeyValue[0].value = "B4A63CDF";   // RAM
            __write_ocpp_Key_Value(ocpp->cout, KeyValue, true);   // RAM value!

            __write_ocpp_Key_Value_i32(ocpp->cout, str_meterStart, 777);
            fprintf_P(ocpp->cout, PSTR(","));
            __write_Time_to_ocpp(ocpp, str_timestamp);
            // wait TransactionId
            break;
            
        case StopTransaction:
            __write_Json_start_P(ocpp->cout, str_StopTransaction);
            __write_ocpp_Key_Value_i32(ocpp->cout, str_transactionId, ocpp->TransactionId);
            fprintf_P(ocpp->cout, PSTR(","));
            __write_ocpp_Key_Value_i32(ocpp->cout, str_meterStop, 778);
            fprintf_P(ocpp->cout, PSTR(","));
            __write_Time_to_ocpp(ocpp, str_timestamp);
            fprintf_P(ocpp->cout, PSTR(","));
            KeyValue[0].key   = str_reason;  // PGM
            KeyValue[0].value = "LOCAL STOP";    // RAM
            __write_ocpp_Key_Value(ocpp->cout, KeyValue, true);    // RAM value!
            break;
        
        case MeterValues:
            __write_Json_start_P(ocpp->cout, str_MeterValues);
            // "connectorId" / "meterValue" / "timestamp"
            // "sampledValue" "value" "measurand" "unit"
            // ["measurand"] = "Energy.Active.Import.Register"; "Wh";
            // ["measurand"] = "Power.Active.Import";  ["unit"] = "W";
            // printf_P(str_MeterValues;
        break;
        
        case SetChargingProfile:            printf_P(str_SetChargingProfile); break;
        case TriggerMessage:                printf_P(str_TriggerMessage); break;
        case RemoteStartTransaction:
            // payload["idTag"] | String("Invalid")); payload["status"] = "Accepted";
        break;
        case RemoteStopTransaction:         printf_P(str_RemoteStopTransaction); break;
        case ChangeConfiguration:           printf_P(str_ChangeConfiguration); break;
        case GetConfiguration:
            // payload["key"] configurationKey, unknownKey, 
        break;
        case Reset:                         printf_P(str_Reset); break;
        case UpdateFirmware:                printf_P(str_UpdateFirmware); break;
        case FirmwareStatusNotification:    printf_P(str_FirmwareStatusNotification); break;
        case GetDiagnostics:                printf_P(str_GetDiagnostics); break;
        case DiagnosticsStatusNotification: printf_P(str_DiagnosticsStatusNotification); break;
        case UnlockConnector:               printf_P(str_UnlockConnector); break;
        case ClearChargingProfile:
            // reply payload["status"] = "Accepted"); Unknown
            // in: id > connectorId > chargingProfilePurpose (ChargePointMaxProfile,
            // TxDefaultProfile, TxProfile) stackLevel
        break;
        case ChangeAvailability:
            // reply payload["status"] = "Rejected", "Scheduled", "Accepted";
        break;
        // ----------------
        case CancelReservation:             printf_P(str_CancelReservation); break;
        case ClearCache:                    printf_P(str_ClearCache); break;
        case DataTransfer:                  printf_P(str_DataTransfer); break;
        case GetLocalListVersion:           printf_P(str_GetLocalListVersion); break;
        case GetCompositeSchedule:          printf_P(str_GetCompositeSchedule); break;
        case ReserveNow:                    printf_P(str_ReserveNow); break;
        case SendLocalList:                 printf_P(str_SendLocalList); break;
        // ----------------
        default: {
            printf_P(PSTR("OCPP_write_req, default(%d)!\n"), ocpp->State);
        }
    }

    fprintf_P(ocpp->cout, PSTR("}]"));
    return 0;//////ms.get_Printed();
}
