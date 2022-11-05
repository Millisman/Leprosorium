#include "ocpp_man.h"
#include "Base64.h"
#include "uuid.h"
#include "app_gsm/gsm_proc.h"
#include "app_charger/adc_mega.h"
#include "mcu/uart.h"
#include "ocpp_parser.h"
#include "sha1.h"

#if __has_include("../../secret.h")
#include "../../secret.h"
#else
const char WSC_host[] PROGMEM = "server.com";
const char WSC_port[] PROGMEM = "80";
const char WSC_path[] PROGMEM = "/ocpp/websocket/CentralSystemService/1";
#endif

#define STACK_SIZE 4

static char bbbbbbbufff[sizeof(sha1_mem)];  // ALERT!
static ocpp_structure_t Ocpp;
static StreamMode_t     OStream;
static FILE             OFile;



#define  LOG_ON
// cout << EOL << uint16_t(__LINE__) << ' ' << __func__ << ' '; x
#ifdef LOG_ON
#define LOGGING(x) x
#else
#define LOGGING(x)
#endif

int MaskingStream_write(char ch, FILE *stream) {
    (void)stream;
    if (OStream.enabled) {
        uart0_putc(ch);
        if (OStream.masking) {
            _hal_modem_write(ch ^ OStream._mask_bytes[OStream.counter % 4]);
        } else {
            _hal_modem_write(ch);
        }
    }
    OStream.counter++;
    return 0;
}

void MaskingStream_print_header() {
    _hal_modem_write(0x81); // fin + opcode txt
    uint8_t x = 0;
    if (OStream.masking) { x = 128; }
    if (OStream.head_size == 2) {
        x |= OStream.body_size;
        _hal_modem_write(x);
    } else if (OStream.head_size == 4) {
        x |= 126;
        _hal_modem_write(x);
        _hal_modem_write((OStream.body_size >> 8) & 0xFF);
        _hal_modem_write(OStream.body_size & 0xFF);
    } else { /* large not support */ }

    if (OStream.masking) {
        for (x = 0; x < sizeof(OStream._mask_bytes); ++x) {
            _hal_modem_write(OStream._mask_bytes[x]);
        }
    }
}


// █ █ █ █ █ █ █ █ █   OCPP_init   █ █ █ █ █ █ █ █ █ █
void OCPP_init() {
    Ocpp.cout = &OFile;
    fdev_setup_stream(Ocpp.cout, MaskingStream_write, NULL, _FDEV_SETUP_WRITE);
    srand(AdcMega_get_val(instant_SET_I));
    srand(AdcMega_get_val(instant_ADC_T1));
    srand(AdcMega_get_val(instant_ADC_T2));
    srand(AdcMega_get_val(instant_ADC_CP));
}

// █ █ █ █ █ █ █ █ █   OCPP_begin   █ █ █ █ █ █ █ █ █ █
void OCPP_begin() {
    srand(AdcMega_get_val(instant_ADC_T1));
    srand(AdcMega_get_val(instant_ADC_T2));
    Ocpp.uuid_n = 1;
    Ocpp.TransactionId = 0;
    memset(Ocpp.req__uuid_36,  0, sizeof(Ocpp.req__uuid_36));
    memset(Ocpp.conf_uuid_36,  0, sizeof(Ocpp.conf_uuid_36));
    memset(&OStream, 0, sizeof(OStream));
    OStream.masking = true;

    uint8_t i;
    for (i = 0; i < sizeof(OStream._mask_bytes); ++i) {
        OStream._mask_bytes[i] = rand() % 256;
    }
    for (i = 0; i < 16; ++i) {
        Ocpp.ws_key.KeyStart[i] = random() % 256;
    }
    for (i = 0; i < sizeof(Ocpp.ws_key.Key); ++i) {
        Ocpp.ws_key.Key[i] = '-';
    }
    Ocpp.ws_key.Key[sizeof(Ocpp.ws_key.Key) - 1] = '\0';

    base64_encode(Ocpp.ws_key.Key, Ocpp.ws_key.KeyStart, 16);


}

const char WSH_GET[]  PROGMEM = "GET ";
const char WSH_HTTP[] PROGMEM = " HTTP/1.1\r\n"
"Upgrade: websocket\r\n" "Connection: Upgrade\r\n"
"Sec-WebSocket-Protocol: ocpp1.6\r\n" "Host: ";
const char WSH_KEY[]  PROGMEM =
"\r\nSec-WebSocket-Version: 13\r\n" "Sec-WebSocket-Key: ";
const char WSH_EOL2[] PROGMEM = "\r\n\r\n";

uint16_t ___OCPP_write_Starting() {

    LOGGING( printf_P(PSTR("OCPP_write_Starting State: %u\n"), Ocpp.State); );

    OStream.to_server = 0;

    switch (Ocpp.State) {

        case NONE_STATE:
            LOGGING( printf_P(PSTR("NONE - Break!\n")); );
            break;

        case Http_Connection_Upgrade:
            OStream.to_server = strlen(Ocpp.ws_key.Key) + 4 +
                (sizeof(WSH_GET) - 1) + // correct \0
                (sizeof(WSC_path) - 1) +
                (sizeof(WSH_HTTP) - 1) +
                (sizeof(WSC_host) - 1) +
                (sizeof(WSH_KEY) - 1);
            break;

        default:
            OStream.enabled = false;
            OStream.counter = 0;
            OCPP_write_req(&Ocpp, false);

            if (OStream.counter < 126) {
                OStream.head_size = 2;
            } else {
                OStream.head_size = 4;
            } // large frame - not support

            if (OStream.masking) { OStream.head_size += sizeof(OStream._mask_bytes); }
            OStream.body_size = OStream.counter;
            OStream.to_server = OStream.body_size + OStream.head_size;
    }
    return OStream.to_server;
}

void ___OCPP_write_Completion() {
    LOGGING( printf_P(PSTR("w end\r\n")); );

    switch (Ocpp.State) {

        case NONE_STATE:
            LOGGING( printf_P(PSTR("NONE_STATE: Break!\n")); );
            break;

        case Http_Connection_Upgrade:
            LOGGING( printf_P(PSTR("Http_Connection_Upgrade\n")); );
            fprintf_P(Ocpp.cout, WSH_GET);
            fprintf_P(Ocpp.cout, WSC_path);
            fprintf_P(Ocpp.cout, WSH_HTTP);
            fprintf_P(Ocpp.cout, WSC_host);
            fprintf_P(Ocpp.cout, WSH_KEY);
            fprintf(Ocpp.cout, "%s", Ocpp.ws_key.Key);
            fprintf_P(Ocpp.cout, PSTR("\r\n\r\n"));
            break;

        default:
            LOGGING( printf_P(PSTR("write_OCPP_state")); );
            OStream.enabled = true;
            MaskingStream_print_header();
            OStream.counter = 0;
            OCPP_write_req(&Ocpp, true);
    }
    OStream.to_server = 0;
}
    
// }

#define WS_CHECKTABLE_COUNT 3
const char   str_websocket[]  PROGMEM = "websocket";
const char   str_Connection[] PROGMEM = "Connection:";
const char     str_upgrade[]  PROGMEM = "upgrade";
const char *ws_checktable[WS_CHECKTABLE_COUNT] = {
    str_upgrade,
    str_websocket,
    str_Connection
};

static uint8_t stack_count;
static OCPP_States contents[STACK_SIZE];

OCPP_States OCPP_States_stack_pop() {
    OCPP_States result = NONE_STATE;
    if (stack_count > 0) { result = contents[--stack_count]; }
    return result;
}

void OCPP_States_stack_push(const OCPP_States c) {
    LOGGING( printf_P(PSTR("Stack push:%u\n"), c); );
    if (stack_count < STACK_SIZE) {
        for (uint8_t x = 0; x < stack_count; x++) {
            if (contents[x] == c) return; // prevent duplicates
        }
        contents[stack_count++] = c;
    } else {
        printf_P(PSTR("\nStack OVF! %u %u\n"), STACK_SIZE, c);
        // TODO append fault handler
    }
}

const char     str_http_11[]  PROGMEM = "HTTP/1.1 101";
const char     str_http_10[]  PROGMEM = "HTTP/1.0 101";
const char str_SecWsAccept[]  PROGMEM = "Sec-WebSocket-Accept:";
const char       str_UUID[]   PROGMEM = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

OCPP_ERR_Processing OCPP_data_processing(uint8_t *payload, uint16_t length) {
    if (!payload || !length) return OCPP_ERR_PTR_SIZE;

    switch (Ocpp.State) {
        case Http_Connection_Upgrade:
            Ocpp.connected = false;

            if ((strstr_P((const char *)payload, str_http_11) != NULL) ||
                (strstr_P((const char *)payload, str_http_10) != NULL))
            {
                uint8_t check = 0;
                for (uint8_t x = 0; x < WS_CHECKTABLE_COUNT; ++x) {
                    if (strcasestr_P((const char *)payload, ws_checktable[x]) != NULL) { check++; }
                    if (check == WS_CHECKTABLE_COUNT) {
                        if (strcasestr_P((const char *)payload, str_SecWsAccept) != NULL) {
                            Sha1_init_173((sha1_mem *)bbbbbbbufff);
                            for (uint8_t a = 0; a < strlen(Ocpp.ws_key.Key); ++a) {
                                Sha1_write(Ocpp.ws_key.Key[a]);
                            }
                            char m;
                            char *mp = (char *)str_UUID;
                            while ((m = pgm_read_byte(mp++))) { Sha1_write(m); }
                            uint8_t *hash = Sha1_result();
                            Ocpp.ws_key.KeyStart[base64_encode(Ocpp.ws_key.KeyStart, (char *)hash, 20)] = '\0';
                            if (strcasestr((const char *)payload, Ocpp.ws_key.KeyStart) != NULL) {
                                OCPP_States_stack_push(BootNotification);
                                Ocpp.connected = true;
                                return OCPP_ERR_OK;
                            } else return OCPP_ERR_KEYSTART;
                        } else return OCPP_ERR_WSACCEPT;
                    } else return OCPP_ERR_CHECKTABLE;
                }
            } else return OCPP_ERR_NO_HTTP10;
            break;

        default :

            if (length >= 2) {
                if ((payload[1] & 0x80) == 0x00) {
                    uint8_t msg_masked = (payload[1] >> 7) & 0x01;
                    uint8_t payload_offset = 2;
                    uint8_t length_field = payload[1] & (~0x80);
                    uint16_t payload_length = 0;
                    if (length_field <= 125) {
                        payload_length = length_field;
                    } else if (length_field == 126) { // msglen is 16bit!
                        payload_length = ( (payload[2] << 8) | (payload[3]) );
                        payload_offset += 2;
                    } else { return OCPP_ERR_WS_LARGE; };
                    if (length < payload_length + payload_offset) { return OCPP_ERR_INCOMPLETE_FRAME; } // INCOMPLETE_FRAME
                    if (msg_masked) {
                        uint32_t mask = *((uint32_t *)(payload + payload_offset));
                        payload_offset += 4;
                        uint8_t *c = payload + payload_offset;
                        for (uint16_t i = 0; i < payload_length; i++) { // unmask payload:
                            c[i] = c[i] ^ ((uint8_t *)(&mask))[i % 4];
                        }
                    }
                    LOGGING(
                        printf_P(PSTR("--check_OCPP_Frame, size: %u\n"), length);
                        for (uint8_t x = 0; x < length; ++x) { printf("%c", payload[x]); }
                        printf_P(PSTR("\n-END-\n"));
                    );
                    if (mjson((char *)payload, length, NULL, NULL) > 0) {
                        Ocpp.json_payload = (char *)payload;
                        Ocpp.json_payload_length = length;
                        return OCPP_ERR_OK;
                    } else return OCPP_ERR_INVALID_JSON;
                } else return OCPP_ERR_WS_FRAME_MASKED;
            } else return OCPP_ERR_SMALL;
    }
    return OCPP_ERR_UNK;
}


bool gs_man_is_Connected() { return true; } // TODO FAKE!
bool gs_man_is_Busy() { return false; } // TODO FAKE!

static ConnStates mConnState = Conn_IDLE;




void OCPP_tick_100ms() {
    if (Ocpp.shed_conf_msg) {
        Ocpp.shed_conf_msg = false;
        LOGGING(
            printf_P(PSTR("OCPP_Parser_processing"));
            print_OCPP_States(Ocpp.State);
        );
        const char *с0, *с1;
        int int0, int1;
        if ((mjson_find(Ocpp.json_payload, Ocpp.json_payload_length, "$[0]", &с0, &int0) == MJSON_TOK_NUMBER) &&
            (mjson_find(Ocpp.json_payload, Ocpp.json_payload_length, "$[1]", &с1, &int1) == MJSON_TOK_STRING) &&
            (int0 == 1) && (int1 >= 36))
        {
            if (mjson_get_string(Ocpp.json_payload, Ocpp.json_payload_length, "$[1]", Ocpp.str_processing_uuid, sizeof(Ocpp.str_processing_uuid)) == 36) {

                if (с0[0] == '2') {
                    /*
                        *                if ((mjson_find(json_payload, json_payload_length, "$[2]", &tmp_char0, &tmp_int0) == MJSON_TOK_STRING)
                        *                    && (tmp_int0 > 0)) {
                        *                    for (uint8_t x = 0; x < OCPP_STATES_COUNT; ++x ) {
                        *                        if (strstr_P(tmp_char0, OCPP_sp[x].label) != NULL) {
                        *                            if (mjson_find(json_payload, json_payload_length, "$[3]", &tmp_char1, &tmp_int1) == MJSON_TOK_OBJECT) {
                        *                                //processing_call(OCPP_sp[x].state, str_processing_uuid, tmp_char1, tmp_int1);
                        *
                        *
                        *                                return;
                }
                }
                }
                }
                */
                } else if (с0[0] == '3') {

                    //                 mjson_find(Ocpp._json_ptr, Ocpp._json_len


//                     const char *tmp_char1;
//                     int tmp_int1;
                    
                    
                    if (mjson_find(Ocpp.json_payload, Ocpp.json_payload_length, "$[2]", &с1, &i1) == MJSON_TOK_OBJECT) {

                        OCPP_Parser_processing_conf(&Ocpp);
                        return;
                    }

                } else if (с0[0] == '4') {
                    OCPP_Parser_processing_error(&Ocpp);
                } else {
                    printf_P(PSTR("unknown message code: %c\n"), с0[0]);
                    //                 LOGGING( printf_P(PSTR("unknown message code: ") << Ocpp.tmp_char0[0] << EOL; );
                }
            }
        } else {

            printf_P(PSTR("Invalid UUID or code\n"));
        }
        //         cout << "Invalid UUID or code\n" << EOL; }
//         }







    } else {

        if (gs_man_is_Connected()) {

            switch (mConnState) {

                case Conn_IDLE:
                    mConnState = Conn_SEND_SIZE;
                    break;

                case Conn_SEND_SIZE:
                    if (gs_man_is_Busy()) {
                        mConnState = Conn_IDLE;
                    } else {
                        Ocpp.State = OCPP_States_stack_pop();

                        if (Ocpp.State == NONE_STATE) {
                            mConnState = Conn_IDLE;
                        } else {

                            if (___OCPP_write_Starting()) {
                                mConnState = Conn_SEND_DATA;
                                // gs_man.send_Schedule(m_ocpp.to_server); ALERT
                            } else { mConnState =Conn_IDLE; }
                        }
                    }
                break;

                case Conn_SEND_DATA:
                    if (!gs_man_is_Busy()) {
                        ___OCPP_write_Completion();
                        mConnState = Conn_IDLE;
                    }
                    break;

            }

        } else { /*not connected :(*/ }
    }
}



void OCPP_tick_1sec() {
    ++Ocpp.UnixTime;
    //         if (online) {
    //
    //             counter_Heartbeat--; // Charge points will send heartbeats roughly every 3 minutes
    //             if (counter_Heartbeat == 0) {
    //                 counter_Heartbeat = 5; // TODO interval set
    //                 stack_push(Heartbeat);
    //             }
    //
    //             counter_Authorize--;
    //             if (counter_Authorize == 0) {
    //                 counter_Authorize = 5; // TODO interval set
    //                 stack_push(Authorize);
    //             }
    //
    //             counter_s1--;
    //             if (counter_s1 == 0) {
    //                 counter_s1 = 10; // TODO interval set
    //                 stack_push(StartTransaction);
    //             }
    //
    //             counter_s2--;
    //             if (counter_s2 == 0) {
    //                 counter_s2 = 20; // TODO interval set
    //                 stack_push(StopTransaction);
    //             }
    //
    // //             counter_StatusNotification--;
    // //             if (counter_StatusNotification == 0) {
    // //                 counter_StatusNotification = 5; // TODO interval set
    // //                 stack_push(StatusNotification);
    // //             }
    //         }
}


//     case events::NFC_CARD_READ_STATE:
//         if (app::NFCrresult::OK == event.param && online) { stack_push(Authorize); }
//         break;

