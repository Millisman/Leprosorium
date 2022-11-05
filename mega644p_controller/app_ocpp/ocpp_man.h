#pragma once
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-attributes"

#include <stdint.h>
#include <stdbool.h>
#include <avr/pgmspace.h>

typedef enum {
    Conn_IDLE,
    Conn_SEND_SIZE,
    Conn_SEND_DATA
} ConnStates;

typedef struct {
    bool modem_connected;
    bool ocpp_started;
    uint16_t sheduled_send;
    uint16_t to_server;
//     bool calculating_node;
        uint8_t /*mH[4],*/ _mask_bytes[4]; //, count_h;
        uint16_t counter;
//         uint16_t cSz;
        bool msk;
        bool hpc;
        bool enabled;
        bool masking;
        uint8_t  head_size;
        uint16_t body_size;
} StreamMode_t;

void OCPP_init();
void OCPP_begin();
void OCPP_tick_1sec();
void OCPP_tick_100ms();

typedef enum {
    OCPP_ERR_OK,
    OCPP_ERR_PTR_SIZE,
    OCPP_ERR_NO_HTTP10,
    OCPP_ERR_CHECKTABLE,
    OCPP_ERR_WSACCEPT,
    OCPP_ERR_KEYSTART,
    OCPP_ERR_SMALL,
    OCPP_ERR_WS_FRAME_MASKED,
    OCPP_ERR_INVALID_JSON,
    OCPP_ERR_UNK,
    OCPP_ERR_WS_LARGE,
    OCPP_ERR_INCOMPLETE_FRAME

} OCPP_ERR_Processing;

OCPP_ERR_Processing OCPP_data_processing(uint8_t *payload, uint16_t length);

