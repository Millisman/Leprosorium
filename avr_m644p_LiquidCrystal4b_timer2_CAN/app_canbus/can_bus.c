#include "mcu/mcu.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "mcu/uart.h"
#include <stdlib.h>
#include <util/delay.h>
#include "mcp2515.h"

void CAN_init() {
    MCP2515_init();
    if (MCP2515_reset() != ERROR_OK ) printf("MCP2515_reset error\n");
    if (MCP2515_setBitrate(CAN_500KBPS) != ERROR_OK ) printf("MCP2515_setBitrate 500 error\n");
    if (MCP2515_setNormalMode() != ERROR_OK ) printf("MCP2515_setNormalMode error\n");
}

static struct can_frame canMsg;

void CAN_update() {

    if (MCP2515_readMessage(&canMsg) == ERROR_OK) {
        printf("Id= 0x%02lX Dlc= 0x%02X", canMsg.can_id, canMsg.can_dlc);

        for (int i = 0; i<canMsg.can_dlc; i++)  {  // print the data
            printf(" %02X", canMsg.data[i]);
        }
        printf("\n");
    }

//     tCAN message;
//     if (mcp2515_check_message()) {
//         if (mcp2515_get_message(&message)) {
//             //uncomment when you want to filter
//             //if(message.id == 0x620 and message.data[2] == 0xFF)
//             //{
//             printf("ID: 0x%2X, Data: %u, 0x", message.id, message.header.length);
//             for( int i=0; i<message.header.length; i++) {
//                 printf("%2X", message.data[i]);
//             }
//             printf("\n");
//         }
//     }
}

void CAN_update_1S() {  }

typedef struct {
    uint8_t     enable;
    uint16_t    power;
    uint16_t    volt;
    uint16_t    current;
} eltek_control;

// 0x2FF
void CAN_update_100ms() {

    struct can_frame canMsg1;

    eltek_control ss; // = {0};

    ss.enable   = 1;
    ss.power    = 1000;
    ss.volt     = 3000;
    ss.current  = 100;

    canMsg1.can_id  = 0x2FF;
    canMsg1.can_dlc = sizeof(ss);
    memcpy(&canMsg1.data, &ss, sizeof(ss));
    //     canMsg1.data[0] = 0x8E;
    //     canMsg1.data[1] = 0x87;
    //     canMsg1.data[2] = 0x32;
    //     canMsg1.data[3] = 0xFA;
    //     canMsg1.data[4] = 0x26;
    //     canMsg1.data[5] = 0x8E;
    //     canMsg1.data[6] = 0xBE;
    //     canMsg1.data[7] = 0x86;
    MCP2515_sendMessage(&canMsg1);
}

