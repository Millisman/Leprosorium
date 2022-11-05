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


uint8_t __calc_10(uint8_t a, uint8_t b) {
    return ((a-'0')*10 + (b-'0'));
}

// {"currentTime":"2022-02-19T03:16:16.633Z"}
bool OCPP_Parser_load_System_UnixTime(ocpp_structure_t *ocpp) {
    const char *tmp_c;
    int   tmp_i;
    if ((mjson_find(ocpp->json_payload, ocpp->json_payload_length, "$.currentTime", &tmp_c, &tmp_i) == MJSON_TOK_STRING)) {
        if (tmp_i > 18) {
            if (isdigit(tmp_c[1]) || isdigit(tmp_c[2]) ||
                isdigit(tmp_c[3]) || isdigit(tmp_c[4]) ||
                tmp_c[5] == '-' ||
                isdigit(tmp_c[6]) || isdigit(tmp_c[7]) ||
                tmp_c[8] == '-' ||
                isdigit(tmp_c[9]) || isdigit(tmp_c[10]) ||
                tmp_c[11] == 'T' ||
                isdigit(tmp_c[12]) || isdigit(tmp_c[13]) ||
                tmp_c[14] == ':' ||
                isdigit(tmp_c[15]) || isdigit(tmp_c[16]) ||
                tmp_c[17] == ':' ||
                isdigit(tmp_c[18]) || isdigit(tmp_c[19]))
            {
                ocpp->rtc.yOff = __calc_10(tmp_c[3] , tmp_c[4] );
                ocpp->rtc.m    = __calc_10(tmp_c[6] , tmp_c[7] );
                ocpp->rtc.d    = __calc_10(tmp_c[9] , tmp_c[10]);
                ocpp->rtc.hh   = __calc_10(tmp_c[12], tmp_c[13]);
                ocpp->rtc.mm   = __calc_10(tmp_c[15], tmp_c[16]);
                ocpp->rtc.ss   = __calc_10(tmp_c[18], tmp_c[19]);
                return true;
            }
        }
    }
    return false;
}




const OCPP_AuthorizationStatus_pair OCPP_AuthStaPair[AUTHORIZATIONSTATUS_COUNT] = {
    { AuthorizationStatus_Accepted,     str_Accepted },
    { AuthorizationStatus_Blocked,      str_Blocked },
    { AuthorizationStatus_Expired,      str_Expired },
    { AuthorizationStatus_Invalid,      str_Invalid },
    { AuthorizationStatus_ConcurrentTx, str_ConcurrentTx }
};

const OCPP_RegistrationStatus_pair OCPP_RegStaPair[REGISTRATIONSTATUS_COUNT] = {
    { RegistrationStatus_Accepted, str_Accepted },
    { RegistrationStatus_Pending,  str_Pending },
    { RegistrationStatus_Rejected, str_Rejected }
};



void OCPP_Parser_processing_conf(ocpp_structure_t *ocpp) {
    // [3,"34a446bc-eea1-4b01-9cb8-b3d7c7d8196d",{}]
    // [3,"1c7b64c5-1473-4ac5-9e4b-79633b706424",{"idTagInfo":{"status":"Accepted","expiryDate":"2023-04-08T00:00:00.000Z"}}]
    // [3,"ea7e64ff-81eb-41fd-bec3-9b67bf0de98c",{"currentTime":"2022-02-17T22:16:56.388Z"}]
    // [3,"f7c064ff-1aff-4ccf-ae7f-e91dd79cb980",{"status":"Accepted","currentTime":"2022-02-11T21:15:25.350Z","interval":14400}]
    printf_P(PSTR("  processing_response ")); print_OCPP_States(ocpp->State);
    printf_P(PSTR("\nprocessing_response UUID: %s\n"), ocpp->conf_uuid_36);
    printf_P(PSTR("  processing_response obj_str: %s\n"), ocpp->json_payload);
    printf_P(PSTR("  processing_response obj_len: %d\n"), ocpp->json_payload_length);

    bool t_present = OCPP_Parser_load_System_UnixTime(ocpp);

    const char *tmp_char0, *tmp_char1;
    int tmp_int0, tmp_int1;

    switch (ocpp->State) {
        
        case Authorize:
            // {"idTagInfo":{"status":"Accepted","expiryDate":"2023-04-08T00:00:00.000Z"}}]
            if ((mjson_find(ocpp->json_payload, ocpp->json_payload_length, "$.idTagInfo", &tmp_char0, &tmp_int0) == MJSON_TOK_OBJECT)) {
                if ((mjson_find(tmp_char0, tmp_int0, "$.status", &tmp_char1, &tmp_int1) == MJSON_TOK_STRING)) {
                    
                    //lcd::LcdConsole &lcdc = lcd::LcdConsole::init();
                    //lcdc.executed = mcu::Timer::millis(); // hold screen
                    
                    for (uint8_t x = 0; x < AUTHORIZATIONSTATUS_COUNT; ++x) {
                        if (strstr_P(tmp_char1, OCPP_AuthStaPair[x].label) != NULL) {
                            ocpp->Authorize_AuthorizationStatus = OCPP_AuthStaPair[x].state;
                            printf_P(PSTR("\nFOUND AuthStaPair STATUS: %s (%u)\n"), OCPP_AuthStaPair[x].label, OCPP_AuthStaPair[x].state);
//                             LOGGING( printf_P(PSTR("\nFOUND AuthStaPair STATUS: ") <<
//                             OCPP_AuthStaPair[x].label << " (" << OCPP_AuthStaPair[x].state << ")\n"; );
                            
                            
//                             event::Loop::post(event::Event(events::NFC_CARD_AUTH_STATE, OCPP_AuthStaPair[x].state));
                            //lcdc.clear_line(0);
                            //lcdc.lcd << OCPP_AuthStaPair[x].label;
                        }
                    }
                    

                    if (mjson_get_string(tmp_char0, tmp_int0, "$.expiryDate", ocpp->Authorize_expiryDate, sizeof(ocpp->Authorize_expiryDate)) > 0) {
                        //LOGGING ( cout << "expiryDate= " << Authorize_expiryDate << EOL;);
                        //lcdc.clear_line(1);
                        //for (uint8_t q = 0; q < 20; ++q) { lcdc.lcd << Authorize_expiryDate[q]; }
                        //lcdc.lcd << OCPP_AuthStaPair[x].label;
                    }
                }
            }
        break;

//         
        
        case BootNotification: { 
            // {"status":"Accepted","currentTime":"2022-02-19T01:07:53.175Z","interval":14400}]
            double m_BufDouble;
            if (t_present) {
                if (mjson_get_number(ocpp->json_payload, ocpp->json_payload_length, "$.interval", &m_BufDouble) != 0) {
                    if ((mjson_find(ocpp->json_payload, ocpp->json_payload_length, "$.status", &tmp_char0, &tmp_int0) == MJSON_TOK_STRING)) {
                        for (uint8_t x = 0; x < REGISTRATIONSTATUS_COUNT; ++x) {
                            if (strstr_P(tmp_char0, OCPP_RegStaPair[x].label) != NULL) {
                                ocpp->BootNotification_RegistrationStatus = OCPP_RegStaPair[x].state;


                                printf_P(PSTR("\nFOUND REGISTRATION STATUS: %s (%u)\n"), OCPP_AuthStaPair[x].label, OCPP_AuthStaPair[x].state);

//                                 LOGGING( printf_P(PSTR("\nFOUND REGISTRATION STATUS: ") <<
//                                 OCPP_RegStaPair[x].label << " (" << OCPP_RegStaPair[x].state << ")\n"; );
                            }
                        }
                    }
                }// else { LOGGING( printf_P(PSTR("'interval' key not found in payload") << obj_str; ); }
            }// else { LOGGING( printf_P(PSTR("'currentTime' key not found in payload") << obj_str; ); }
        }
        break;

        case Heartbeat:
        break;


// //         printf_P(str_MeterValues);

        case MeterValues:                   printf_P(str_MeterValues); break;
        case SetChargingProfile:            printf_P(str_SetChargingProfile); break;
        case StatusNotification:            printf_P(str_StatusNotification); break;
        case StartTransaction:
            printf_P(str_StartTransaction);
//             cout << "============================ ";
//             cout << "############################ ";
            
            break;
        case StopTransaction:
            printf_P(str_StartTransaction);
//             cout << "========$$$$$$$$$$$$==== ";
//             cout << "$$$$$$$$$$$$$$$$$$$$$$$$ ";
            
            break;
        case TriggerMessage:                printf_P(str_TriggerMessage); break;
        case RemoteStartTransaction:        printf_P(str_RemoteStartTransaction); break;
        case RemoteStopTransaction:         printf_P(str_RemoteStopTransaction); break;
        case ChangeConfiguration:           printf_P(str_ChangeConfiguration); break;
        case GetConfiguration:              printf_P(str_GetConfiguration); break;
        case Reset:                         printf_P(str_Reset); break;
        case UpdateFirmware:                printf_P(str_UpdateFirmware); break;
        case FirmwareStatusNotification:    printf_P(str_FirmwareStatusNotification); break;
        case GetDiagnostics:                printf_P(str_GetDiagnostics); break;
        case DiagnosticsStatusNotification: printf_P(str_DiagnosticsStatusNotification); break;
        case UnlockConnector:               printf_P(str_UnlockConnector); break;
        case ClearChargingProfile:          printf_P(str_ClearChargingProfile); break;
        case ChangeAvailability:            printf_P(str_ChangeAvailability); break;
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
            
        }
    }
}
