#pragma once

#include <stdint.h>
#include "can.h"

// #define MCP2515_CAN_CLOCK_20MHZ
// #define MCP2515_CAN_CLOCK_16MHZ
#define MCP2515_CAN_CLOCK_8MHZ

// Speed 8M
#define MCP_8MHz_1000kBPS_CFG1 (0x00)
#define MCP_8MHz_1000kBPS_CFG2 (0x80)
#define MCP_8MHz_1000kBPS_CFG3 (0x80)
#define MCP_8MHz_500kBPS_CFG1 (0x00)
#define MCP_8MHz_500kBPS_CFG2 (0x90)
#define MCP_8MHz_500kBPS_CFG3 (0x82)
#define MCP_8MHz_250kBPS_CFG1 (0x00)
#define MCP_8MHz_250kBPS_CFG2 (0xB1)
#define MCP_8MHz_250kBPS_CFG3 (0x85)
#define MCP_8MHz_200kBPS_CFG1 (0x00)
#define MCP_8MHz_200kBPS_CFG2 (0xB4)
#define MCP_8MHz_200kBPS_CFG3 (0x86)
#define MCP_8MHz_125kBPS_CFG1 (0x01)
#define MCP_8MHz_125kBPS_CFG2 (0xB1)
#define MCP_8MHz_125kBPS_CFG3 (0x85)
#define MCP_8MHz_100kBPS_CFG1 (0x01)
#define MCP_8MHz_100kBPS_CFG2 (0xB4)
#define MCP_8MHz_100kBPS_CFG3 (0x86)
#define MCP_8MHz_80kBPS_CFG1 (0x01)
#define MCP_8MHz_80kBPS_CFG2 (0xBF)
#define MCP_8MHz_80kBPS_CFG3 (0x87)
#define MCP_8MHz_50kBPS_CFG1 (0x03)
#define MCP_8MHz_50kBPS_CFG2 (0xB4)
#define MCP_8MHz_50kBPS_CFG3 (0x86)
#define MCP_8MHz_40kBPS_CFG1 (0x03)
#define MCP_8MHz_40kBPS_CFG2 (0xBF)
#define MCP_8MHz_40kBPS_CFG3 (0x87)
#define MCP_8MHz_33k3BPS_CFG1 (0x47)
#define MCP_8MHz_33k3BPS_CFG2 (0xE2)
#define MCP_8MHz_33k3BPS_CFG3 (0x85)
#define MCP_8MHz_31k25BPS_CFG1 (0x07)
#define MCP_8MHz_31k25BPS_CFG2 (0xA4)
#define MCP_8MHz_31k25BPS_CFG3 (0x84)
#define MCP_8MHz_20kBPS_CFG1 (0x07)
#define MCP_8MHz_20kBPS_CFG2 (0xBF)
#define MCP_8MHz_20kBPS_CFG3 (0x87)
#define MCP_8MHz_10kBPS_CFG1 (0x0F)
#define MCP_8MHz_10kBPS_CFG2 (0xBF)
#define MCP_8MHz_10kBPS_CFG3 (0x87)
#define MCP_8MHz_5kBPS_CFG1 (0x1F)
#define MCP_8MHz_5kBPS_CFG2 (0xBF)
#define MCP_8MHz_5kBPS_CFG3 (0x87)

// speed 16M
#define MCP_16MHz_1000kBPS_CFG1 (0x00)
#define MCP_16MHz_1000kBPS_CFG2 (0xD0)
#define MCP_16MHz_1000kBPS_CFG3 (0x82)
#define MCP_16MHz_500kBPS_CFG1 (0x00)
#define MCP_16MHz_500kBPS_CFG2 (0xF0)
#define MCP_16MHz_500kBPS_CFG3 (0x86)
#define MCP_16MHz_250kBPS_CFG1 (0x41)
#define MCP_16MHz_250kBPS_CFG2 (0xF1)
#define MCP_16MHz_250kBPS_CFG3 (0x85)
#define MCP_16MHz_200kBPS_CFG1 (0x01)
#define MCP_16MHz_200kBPS_CFG2 (0xFA)
#define MCP_16MHz_200kBPS_CFG3 (0x87)
#define MCP_16MHz_125kBPS_CFG1 (0x03)
#define MCP_16MHz_125kBPS_CFG2 (0xF0)
#define MCP_16MHz_125kBPS_CFG3 (0x86)
#define MCP_16MHz_100kBPS_CFG1 (0x03)
#define MCP_16MHz_100kBPS_CFG2 (0xFA)
#define MCP_16MHz_100kBPS_CFG3 (0x87)
#define MCP_16MHz_80kBPS_CFG1 (0x03)
#define MCP_16MHz_80kBPS_CFG2 (0xFF)
#define MCP_16MHz_80kBPS_CFG3 (0x87)
#define MCP_16MHz_83k3BPS_CFG1 (0x03)
#define MCP_16MHz_83k3BPS_CFG2 (0xBE)
#define MCP_16MHz_83k3BPS_CFG3 (0x07)
#define MCP_16MHz_50kBPS_CFG1 (0x07)
#define MCP_16MHz_50kBPS_CFG2 (0xFA)
#define MCP_16MHz_50kBPS_CFG3 (0x87)
#define MCP_16MHz_40kBPS_CFG1 (0x07)
#define MCP_16MHz_40kBPS_CFG2 (0xFF)
#define MCP_16MHz_40kBPS_CFG3 (0x87)
#define MCP_16MHz_33k3BPS_CFG1 (0x4E)
#define MCP_16MHz_33k3BPS_CFG2 (0xF1)
#define MCP_16MHz_33k3BPS_CFG3 (0x85)
#define MCP_16MHz_20kBPS_CFG1 (0x0F)
#define MCP_16MHz_20kBPS_CFG2 (0xFF)
#define MCP_16MHz_20kBPS_CFG3 (0x87)
#define MCP_16MHz_10kBPS_CFG1 (0x1F)
#define MCP_16MHz_10kBPS_CFG2 (0xFF)
#define MCP_16MHz_10kBPS_CFG3 (0x87)
#define MCP_16MHz_5kBPS_CFG1 (0x3F)
#define MCP_16MHz_5kBPS_CFG2 (0xFF)
#define MCP_16MHz_5kBPS_CFG3 (0x87)

// speed 20M
#define MCP_20MHz_1000kBPS_CFG1 (0x00)
#define MCP_20MHz_1000kBPS_CFG2 (0xD9)
#define MCP_20MHz_1000kBPS_CFG3 (0x82)
#define MCP_20MHz_500kBPS_CFG1 (0x00)
#define MCP_20MHz_500kBPS_CFG2 (0xFA)
#define MCP_20MHz_500kBPS_CFG3 (0x87)
#define MCP_20MHz_250kBPS_CFG1 (0x41)
#define MCP_20MHz_250kBPS_CFG2 (0xFB)
#define MCP_20MHz_250kBPS_CFG3 (0x86)
#define MCP_20MHz_200kBPS_CFG1 (0x01)
#define MCP_20MHz_200kBPS_CFG2 (0xFF)
#define MCP_20MHz_200kBPS_CFG3 (0x87)
#define MCP_20MHz_125kBPS_CFG1 (0x03)
#define MCP_20MHz_125kBPS_CFG2 (0xFA)
#define MCP_20MHz_125kBPS_CFG3 (0x87)
#define MCP_20MHz_100kBPS_CFG1 (0x04)
#define MCP_20MHz_100kBPS_CFG2 (0xFA)
#define MCP_20MHz_100kBPS_CFG3 (0x87)
#define MCP_20MHz_83k3BPS_CFG1 (0x04)
#define MCP_20MHz_83k3BPS_CFG2 (0xFE)
#define MCP_20MHz_83k3BPS_CFG3 (0x87)
#define MCP_20MHz_80kBPS_CFG1 (0x04)
#define MCP_20MHz_80kBPS_CFG2 (0xFF)
#define MCP_20MHz_80kBPS_CFG3 (0x87)
#define MCP_20MHz_50kBPS_CFG1 (0x09)
#define MCP_20MHz_50kBPS_CFG2 (0xFA)
#define MCP_20MHz_50kBPS_CFG3 (0x87)
#define MCP_20MHz_40kBPS_CFG1 (0x09)
#define MCP_20MHz_40kBPS_CFG2 (0xFF)
#define MCP_20MHz_40kBPS_CFG3 (0x87)
#define MCP_20MHz_33k3BPS_CFG1 (0x0B)
#define MCP_20MHz_33k3BPS_CFG2 (0xFF)
#define MCP_20MHz_33k3BPS_CFG3 (0x87)



typedef enum  {
    CAN_5KBPS,
    CAN_10KBPS,
    CAN_20KBPS,
    CAN_31K25BPS,
    CAN_33KBPS,
    CAN_40KBPS,
    CAN_50KBPS,
    CAN_80KBPS,
    CAN_83K3BPS,
    CAN_95KBPS,
    CAN_100KBPS,
    CAN_125KBPS,
    CAN_200KBPS,
    CAN_250KBPS,
    CAN_500KBPS,
    CAN_1000KBPS
} MCP2515_CAN_SPEED;

typedef enum  {
    CLKOUT_DISABLE = -1,
    CLKOUT_DIV1 = 0x0,
    CLKOUT_DIV2 = 0x1,
    CLKOUT_DIV4 = 0x2,
    CLKOUT_DIV8 = 0x3,
} MCP2515_CAN_CLKOUT;

// class MCP2515
// {
//     public:
typedef enum {
    ERROR_OK        = 0,
    ERROR_FAIL      = 1,
    ERROR_ALLTXBUSY = 2,
    ERROR_FAILINIT  = 3,
    ERROR_FAILTX    = 4,
    ERROR_NOMSG     = 5
} MCP2515_ERROR;

typedef enum  {
    MASK0,
    MASK1
} MCP2515_MASK;

typedef enum  {
    RXF0 = 0,
    RXF1 = 1,
    RXF2 = 2,
    RXF3 = 3,
    RXF4 = 4,
    RXF5 = 5
} MCP2515_RXF;

typedef enum  {
    RXB0 = 0,
    RXB1 = 1
} MCP2515_RXBn;

typedef enum  {
    TXB0 = 0,
    TXB1 = 1,
    TXB2 = 2
} MCP2515_TXBn;

typedef enum /*class*/  : uint8_t {
    CANINTF_RX0IF = 0x01,
    CANINTF_RX1IF = 0x02,
    CANINTF_TX0IF = 0x04,
    CANINTF_TX1IF = 0x08,
    CANINTF_TX2IF = 0x10,
    CANINTF_ERRIF = 0x20,
    CANINTF_WAKIF = 0x40,
    CANINTF_MERRF = 0x80
} MCP2515_CANINTF;

typedef enum /*class*/  : uint8_t {
    EFLG_RX1OVR = (1<<7),
    EFLG_RX0OVR = (1<<6),
    EFLG_TXBO   = (1<<5),
    EFLG_TXEP   = (1<<4),
    EFLG_RXEP   = (1<<3),
    EFLG_TXWAR  = (1<<2),
    EFLG_RXWAR  = (1<<1),
    EFLG_EWARN  = (1<<0)
} MCP2515_EFLG;

//     private:
static const uint8_t CANCTRL_REQOP = 0xE0;
static const uint8_t CANCTRL_ABAT = 0x10;
static const uint8_t CANCTRL_OSM = 0x08;
static const uint8_t CANCTRL_CLKEN = 0x04;
static const uint8_t CANCTRL_CLKPRE = 0x03;

typedef enum /*class*/  : uint8_t {
    CANCTRL_REQOP_NORMAL     = 0x00,
    CANCTRL_REQOP_SLEEP      = 0x20,
    CANCTRL_REQOP_LOOPBACK   = 0x40,
    CANCTRL_REQOP_LISTENONLY = 0x60,
    CANCTRL_REQOP_CONFIG     = 0x80,
    CANCTRL_REQOP_POWERUP    = 0xE0
} MCP2515_CANCTRL_REQOP_MODE;

static const uint8_t CANSTAT_OPMOD = 0xE0;
static const uint8_t CANSTAT_ICOD = 0x0E;
static const uint8_t CNF3_SOF = 0x80;
static const uint8_t TXB_EXIDE_MASK = 0x08;
static const uint8_t DLC_MASK       = 0x0F;
static const uint8_t RTR_MASK       = 0x40;
static const uint8_t RXBnCTRL_RXM_STD    = 0x20;
static const uint8_t RXBnCTRL_RXM_EXT    = 0x40;
static const uint8_t RXBnCTRL_RXM_STDEXT = 0x00;
static const uint8_t RXBnCTRL_RXM_MASK   = 0x60;
static const uint8_t RXBnCTRL_RTR        = 0x08;
static const uint8_t RXB0CTRL_BUKT       = 0x04;
static const uint8_t RXB0CTRL_FILHIT_MASK = 0x03;
static const uint8_t RXB1CTRL_FILHIT_MASK = 0x07;
static const uint8_t RXB0CTRL_FILHIT = 0x00;
static const uint8_t RXB1CTRL_FILHIT = 0x01;
static const uint8_t MCP_SIDH = 0;
static const uint8_t MCP_SIDL = 1;
static const uint8_t MCP_EID8 = 2;
static const uint8_t MCP_EID0 = 3;
static const uint8_t MCP_DLC  = 4;
static const uint8_t MCP_DATA = 5;

typedef enum /*class*/  : uint8_t {
    STAT_RX0IF = (1<<0),
    STAT_RX1IF = (1<<1)
} MCP2515_STAT;

static const uint8_t STAT_RXIF_MASK = STAT_RX0IF | STAT_RX1IF;

typedef enum /*class*/  : uint8_t {
    TXB_ABTF   = 0x40,
    TXB_MLOA   = 0x20,
    TXB_TXERR  = 0x10,
    TXB_TXREQ  = 0x08,
    TXB_TXIE   = 0x04,
    TXB_TXP    = 0x03
} MCP2515_TXBnCTRL;

static const uint8_t EFLG_ERRORMASK = EFLG_RX1OVR
| EFLG_RX0OVR
| EFLG_TXBO
| EFLG_TXEP
| EFLG_RXEP;

typedef enum /*class*/  : uint8_t {
    INSTRUCTION_WRITE       = 0x02,
    INSTRUCTION_READ        = 0x03,
    INSTRUCTION_BITMOD      = 0x05,
    INSTRUCTION_LOAD_TX0    = 0x40,
    INSTRUCTION_LOAD_TX1    = 0x42,
    INSTRUCTION_LOAD_TX2    = 0x44,
    INSTRUCTION_RTS_TX0     = 0x81,
    INSTRUCTION_RTS_TX1     = 0x82,
    INSTRUCTION_RTS_TX2     = 0x84,
    INSTRUCTION_RTS_ALL     = 0x87,
    INSTRUCTION_READ_RX0    = 0x90,
    INSTRUCTION_READ_RX1    = 0x94,
    INSTRUCTION_READ_STATUS = 0xA0,
    INSTRUCTION_RX_STATUS   = 0xB0,
    INSTRUCTION_RESET       = 0xC0
} MCP2515_INSTRUCTION;

typedef enum /*class*/  : uint8_t {
    MCP_RXF0SIDH = 0x00,
    MCP_RXF0SIDL = 0x01,
    MCP_RXF0EID8 = 0x02,
    MCP_RXF0EID0 = 0x03,
    MCP_RXF1SIDH = 0x04,
    MCP_RXF1SIDL = 0x05,
    MCP_RXF1EID8 = 0x06,
    MCP_RXF1EID0 = 0x07,
    MCP_RXF2SIDH = 0x08,
    MCP_RXF2SIDL = 0x09,
    MCP_RXF2EID8 = 0x0A,
    MCP_RXF2EID0 = 0x0B,
    MCP_CANSTAT  = 0x0E,
    MCP_CANCTRL  = 0x0F,
    MCP_RXF3SIDH = 0x10,
    MCP_RXF3SIDL = 0x11,
    MCP_RXF3EID8 = 0x12,
    MCP_RXF3EID0 = 0x13,
    MCP_RXF4SIDH = 0x14,
    MCP_RXF4SIDL = 0x15,
    MCP_RXF4EID8 = 0x16,
    MCP_RXF4EID0 = 0x17,
    MCP_RXF5SIDH = 0x18,
    MCP_RXF5SIDL = 0x19,
    MCP_RXF5EID8 = 0x1A,
    MCP_RXF5EID0 = 0x1B,
    MCP_TEC      = 0x1C,
    MCP_REC      = 0x1D,
    MCP_RXM0SIDH = 0x20,
    MCP_RXM0SIDL = 0x21,
    MCP_RXM0EID8 = 0x22,
    MCP_RXM0EID0 = 0x23,
    MCP_RXM1SIDH = 0x24,
    MCP_RXM1SIDL = 0x25,
    MCP_RXM1EID8 = 0x26,
    MCP_RXM1EID0 = 0x27,
    MCP_CNF3     = 0x28,
    MCP_CNF2     = 0x29,
    MCP_CNF1     = 0x2A,
    MCP_CANINTE  = 0x2B,
    MCP_CANINTF  = 0x2C,
    MCP_EFLG     = 0x2D,
    MCP_TXB0CTRL = 0x30,
    MCP_TXB0SIDH = 0x31,
    MCP_TXB0SIDL = 0x32,
    MCP_TXB0EID8 = 0x33,
    MCP_TXB0EID0 = 0x34,
    MCP_TXB0DLC  = 0x35,
    MCP_TXB0DATA = 0x36,
    MCP_TXB1CTRL = 0x40,
    MCP_TXB1SIDH = 0x41,
    MCP_TXB1SIDL = 0x42,
    MCP_TXB1EID8 = 0x43,
    MCP_TXB1EID0 = 0x44,
    MCP_TXB1DLC  = 0x45,
    MCP_TXB1DATA = 0x46,
    MCP_TXB2CTRL = 0x50,
    MCP_TXB2SIDH = 0x51,
    MCP_TXB2SIDL = 0x52,
    MCP_TXB2EID8 = 0x53,
    MCP_TXB2EID0 = 0x54,
    MCP_TXB2DLC  = 0x55,
    MCP_TXB2DATA = 0x56,
    MCP_RXB0CTRL = 0x60,
    MCP_RXB0SIDH = 0x61,
    MCP_RXB0SIDL = 0x62,
    MCP_RXB0EID8 = 0x63,
    MCP_RXB0EID0 = 0x64,
    MCP_RXB0DLC  = 0x65,
    MCP_RXB0DATA = 0x66,
    MCP_RXB1CTRL = 0x70,
    MCP_RXB1SIDH = 0x71,
    MCP_RXB1SIDL = 0x72,
    MCP_RXB1EID8 = 0x73,
    MCP_RXB1EID0 = 0x74,
    MCP_RXB1DLC  = 0x75,
    MCP_RXB1DATA = 0x76
} MCP2515_REGISTER;

#define MCP2515_N_TXBUFFERS 3
#define MCP2515_N_RXBUFFERS 2

// // static const
typedef struct { //
    MCP2515_REGISTER CTRL;
    MCP2515_REGISTER SIDH;
    MCP2515_REGISTER DATA;
} MCP2515_TXBn_REGS; // [MCP2515_N_TXBUFFERS]
//
// // static const
typedef struct { //
    MCP2515_REGISTER CTRL;
    MCP2515_REGISTER SIDH;
    MCP2515_REGISTER DATA;
    MCP2515_CANINTF  CANINTF_RXnIF;
} MCP2515_RXBn_REGS; // [MCP2515_N_RXBUFFERS]

// uint8_t SPICS;
// uint32_t SPI_CLOCK;

//     private:

// void _MCP2515_startSPI();
// void _MCP2515_endSPI();

MCP2515_ERROR _MCP2515_setMode(const MCP2515_CANCTRL_REQOP_MODE mode);
uint8_t _MCP2515_readRegister(const MCP2515_REGISTER reg);
void _MCP2515_readRegisters(const MCP2515_REGISTER reg, uint8_t values[], const uint8_t n);
void _MCP2515_setRegister(const MCP2515_REGISTER reg, const uint8_t value);
void _MCP2515_setRegistersN(const MCP2515_REGISTER reg, const uint8_t values[], const uint8_t n);
void _MCP2515_modifyRegister(const MCP2515_REGISTER reg, const uint8_t mask, const uint8_t data);

void _MCP2515_prepareId(uint8_t *buffer, const uint8_t ext, const uint32_t id);

//     public:
void MCP2515_init();
MCP2515_ERROR MCP2515_reset(void);
MCP2515_ERROR MCP2515_setConfigMode();
MCP2515_ERROR MCP2515_setListenOnlyMode();
MCP2515_ERROR MCP2515_setSleepMode();
MCP2515_ERROR MCP2515_setLoopbackMode();
MCP2515_ERROR MCP2515_setNormalMode();
MCP2515_ERROR MCP2515_setClkOut(const MCP2515_CAN_CLKOUT divisor);
MCP2515_ERROR MCP2515_setBitrate(const MCP2515_CAN_SPEED canSpeed);
// MCP2515_ERROR MCP2515_setBitrateCLK(const MCP2515_CAN_SPEED canSpeed, const MCP2515_CAN_CLOCK canClock);
MCP2515_ERROR MCP2515_setFilterMask(const MCP2515_MASK num, const uint8_t ext, const uint32_t ulData);
MCP2515_ERROR MCP2515_setFilter(const MCP2515_RXF num, const uint8_t ext, const uint32_t ulData);
MCP2515_ERROR MCP2515_sendMessageTxBN(const MCP2515_TXBn txbn, const struct can_frame *frame);
MCP2515_ERROR MCP2515_sendMessage(const struct can_frame *frame);
MCP2515_ERROR MCP2515_readMessageRxBN(const MCP2515_RXBn rxbn, struct can_frame *frame);
MCP2515_ERROR MCP2515_readMessage(struct can_frame *frame);
uint8_t    MCP2515_checkReceive(void);
uint8_t MCP2515_checkError(void);
uint8_t MCP2515_getErrorFlags(void);
void    MCP2515_clearRXnOVRFlags(void);
uint8_t MCP2515_getInterrupts(void);
uint8_t MCP2515_getInterruptMask(void);
void    MCP2515_clearInterrupts(void);
void    MCP2515_clearTXInterrupts(void);
uint8_t MCP2515_getStatus(void);
void    MCP2515_clearRXnOVR(void);
void    MCP2515_clearMERR();
void    MCP2515_clearERRIF();
uint8_t MCP2515_errorCountRX(void);
uint8_t MCP2515_errorCountTX(void);
// };

// #endif
