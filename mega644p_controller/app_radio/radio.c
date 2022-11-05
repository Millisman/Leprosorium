#include "radio.h"
#include <stdio.h>
#include <stdlib.h>

#include <util/delay.h>
#include "mcu/mcu.h"
#include <stdbool.h>
#include "nrf24.h"

uint8_t address[5] = { 0x00, 0x00, 0x00, 0x03, 0xE8 };

// █ █ █ █ █ █ █ █ █ █ █ █ █ █ █ █ █ █ █ █ █ █ █ █ █

bool _radio_present = false;

void app_Radio_init() {
    printf("app_RADIO_init\n");

    nRF24_CE_L();

    if (!nRF24_Check()) {
        printf("nRF24 FAIL\r\n");
//         while (1) {
//
//         }
    } else {
    _radio_present = true;
    // Initialize the nRF24L01 to its default state
    nRF24_Init();

    // Disable ShockBurst for all RX pipes
    nRF24_DisableAA(0xFF);

    // Set RF channel
    nRF24_SetRFChannel(50);

    // Set data rate
    nRF24_SetDataRate(nRF24_DR_1Mbps);

    // Set CRC scheme
    nRF24_SetCRCScheme(nRF24_CRC_2byte);

    // Set address width, its common for all pipes (RX and TX)
    nRF24_SetAddrWidth(5);

    // Configure RX PIPE#1
    // static const uint8_t nRF24_ADDR[] = { 0xE7, 0x1C, 0xE3 };
    nRF24_SetAddr(nRF24_PIPE1, address); // program address for RX pipe #1
    nRF24_SetRXPipe(nRF24_PIPE1, nRF24_AA_OFF, 32); // Auto-ACK: disabled, payload length: 5 bytes

    // Set operational mode (PRX == receiver)
    nRF24_SetOperationalMode(nRF24_MODE_RX);

    // Wake the transceiver
    nRF24_SetPowerMode(nRF24_PWR_UP);

    // Put the transceiver to the RX mode

    printf("..done\n");
    }
    nRF24_CE_H();

}

void app_Radio_update() {}
void app_Radio_update_1s() {}

uint8_t m_Rec_pipe_num;
// ---------------- phase voltage/current data from limiter --------------//
int16_t  m_10xVL1, m_10xVL2, m_10xVL3;
int32_t  m_IrmsL1, m_IrmsL2, m_IrmsL3;
int32_t  m_VAL1, m_VAL2, m_VAL3;
// ---------------- !phase voltage/current data from limiter --------------//
uint16_t    m_StatVoltage;
int32_t     m_AllowVA;
int32_t     m_AllowI;
int8_t      TempC;
uint8_t nRF24_payload[32];
// Length of received payload
uint8_t payload_length;
// Pipe number
nRF24_RXResult pipe;

RF_data_t   m_limData;

const char *m = " L %01d: %03dV %03dA %06dVA";

void app_Radio_update_100ms() {
    if (_radio_present) {
    // Constantly poll the status of the RX FIFO and get a payload if FIFO is not empty
    // This is far from best solution, but it's ok for testing purposes
    // More smart way is to use the IRQ pin :)
    if (nRF24_GetStatus_RXFIFO() != nRF24_STATUS_RXFIFO_EMPTY) {
        // Get a payload from the transceiver
        pipe = nRF24_ReadPayload((uint8_t *)&m_limData, &payload_length);
        // Clear all pending IRQ flags
        nRF24_ClearIRQFlags();
        m_10xVL1 = abs(m_limData.L1_Vrms);
        m_IrmsL1 = abs(m_limData.L1_Irms);
        m_VAL1   = abs(m_limData.L1_apparentPower);
        m_10xVL2 = abs(m_limData.L2_Vrms);
        m_IrmsL2 = abs(m_limData.L2_Irms);
        m_VAL2   = abs(m_limData.L2_apparentPower);
        m_10xVL3 = abs(m_limData.L3_Vrms);
        m_IrmsL3 = abs(m_limData.L3_Irms);
        m_VAL3   = abs(m_limData.L3_apparentPower);

//         printf(m, 1, (int16_t)m_10xVL1/10, (int16_t)m_IrmsL1/1000, m_VAL1);
//         printf(m, 2, (int16_t)m_10xVL2/10, (int16_t)m_IrmsL2/1000, m_VAL2);
//         printf(m, 3, (int16_t)m_10xVL3/10, (int16_t)m_IrmsL3/1000, m_VAL3);
//         printf("\n");
    }
    }
}


