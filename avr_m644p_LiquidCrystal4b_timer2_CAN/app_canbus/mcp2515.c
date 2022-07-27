#include "mcp2515.h"
#include "mcu/mcu.h"
#include "mcu/spimaster.h"
#include "mcu/pins.h"
#include "mcu/pin_macros.h"

#include <string.h>
#include <util/delay.h>

// Usage:
// MCP2515_init();
// MCP2515_reset();
// MCP2515.setBitrate(CAN_125KBPS);
// MCP2515.setNormalMode();

static SPI_regs_t __SPI_REG_MCP2515;

static const MCP2515_TXBn_REGS MCP2515_TXB[MCP2515_N_TXBUFFERS]= {
    {MCP_TXB0CTRL, MCP_TXB0SIDH, MCP_TXB0DATA},
    {MCP_TXB1CTRL, MCP_TXB1SIDH, MCP_TXB1DATA},
    {MCP_TXB2CTRL, MCP_TXB2SIDH, MCP_TXB2DATA}
};

static const MCP2515_RXBn_REGS MCP2515_RXB[MCP2515_N_RXBUFFERS]= {
    {MCP_RXB0CTRL, MCP_RXB0SIDH, MCP_RXB0DATA, CANINTF_RX0IF},
    {MCP_RXB1CTRL, MCP_RXB1SIDH, MCP_RXB1DATA, CANINTF_RX1IF}
};

void MCP2515_init() {
    SPI_Master_Init( &__SPI_REG_MCP2515, SPI_MODE0, SPI_CLOCK_DIV16, MSBFIRST);
    ON(PIN_RF24_CSN);
}

MCP2515_ERROR MCP2515_reset(void) {
    SPI_Master_StartTransmission(__SPI_REG_MCP2515, OFF(PIN_RF24_CSN));
    SPI_Master_Transfer8_TxRx(INSTRUCTION_RESET);
    SPI_Master_EndTransmission(ON(PIN_RF24_CSN));
    _delay_ms(10);
    uint8_t zeros[14];
    memset(zeros, 0, sizeof(zeros));
    _MCP2515_setRegistersN(MCP_TXB0CTRL, zeros, 14);
    _MCP2515_setRegistersN(MCP_TXB1CTRL, zeros, 14);
    _MCP2515_setRegistersN(MCP_TXB2CTRL, zeros, 14);
    _MCP2515_setRegister(MCP_RXB0CTRL, 0);
    _MCP2515_setRegister(MCP_RXB1CTRL, 0);
    _MCP2515_setRegister(MCP_CANINTE, CANINTF_RX0IF | CANINTF_RX1IF | CANINTF_ERRIF | CANINTF_MERRF);

    // receives all valid messages using either Standard or Extended Identifiers that
    // meet filter criteria. RXF0 is applied for RXB0, RXF1 is applied for RXB1
    _MCP2515_modifyRegister(MCP_RXB0CTRL,
                   RXBnCTRL_RXM_MASK | RXB0CTRL_BUKT | RXB0CTRL_FILHIT_MASK,
                   RXBnCTRL_RXM_STDEXT | RXB0CTRL_BUKT | RXB0CTRL_FILHIT);
    _MCP2515_modifyRegister(MCP_RXB1CTRL,
                   RXBnCTRL_RXM_MASK | RXB1CTRL_FILHIT_MASK,
                   RXBnCTRL_RXM_STDEXT | RXB1CTRL_FILHIT);

    // clear filters and masks
    // do not filter any standard frames for RXF0 used by RXB0
    // do not filter any extended frames for RXF1 used by RXB1
    MCP2515_RXF filters[] = {RXF0, RXF1, RXF2, RXF3, RXF4, RXF5};
    for (int i=0; i<6; i++) {
        uint8_t ext = !!(i == 1);
        MCP2515_ERROR result = MCP2515_setFilter(filters[i], ext, 0);
        if (result != ERROR_OK) {
            return result;
        }
    }

    MCP2515_MASK masks[] = {MASK0, MASK1};
    for (int i=0; i<2; i++) {
        MCP2515_ERROR result = MCP2515_setFilterMask(masks[i], 1, 0);
        if (result != ERROR_OK) {
            return result;
        }
    }

    return ERROR_OK;
}

uint8_t _MCP2515_readRegister(const MCP2515_REGISTER reg)
{
    SPI_Master_StartTransmission(__SPI_REG_MCP2515, OFF(PIN_RF24_CSN));
    SPI_Master_Transfer8_TxRx(INSTRUCTION_READ);
    SPI_Master_Transfer8_TxRx(reg);
    uint8_t ret = SPI_Master_Transfer8_TxRx(0x00);
    SPI_Master_EndTransmission(ON(PIN_RF24_CSN));

    return ret;
}

void _MCP2515_readRegisters(const MCP2515_REGISTER reg, uint8_t values[], const uint8_t n)
{
    SPI_Master_StartTransmission(__SPI_REG_MCP2515, OFF(PIN_RF24_CSN));
    SPI_Master_Transfer8_TxRx(INSTRUCTION_READ);
    SPI_Master_Transfer8_TxRx(reg);
    // mcp2515 has auto-increment of address-pointer
    for (uint8_t i=0; i<n; i++) {
        values[i] = SPI_Master_Transfer8_TxRx(0x00);
    }
    SPI_Master_EndTransmission(ON(PIN_RF24_CSN));
}

void _MCP2515_setRegister(const MCP2515_REGISTER reg, const uint8_t value)
{
    SPI_Master_StartTransmission(__SPI_REG_MCP2515, OFF(PIN_RF24_CSN));
    SPI_Master_Transfer8_TxRx(INSTRUCTION_WRITE);
    SPI_Master_Transfer8_TxRx(reg);
    SPI_Master_Transfer8_TxRx(value);
    SPI_Master_EndTransmission(ON(PIN_RF24_CSN));
}

void _MCP2515_setRegistersN(const MCP2515_REGISTER reg, const uint8_t values[], const uint8_t n)
{
    SPI_Master_StartTransmission(__SPI_REG_MCP2515, OFF(PIN_RF24_CSN));
    SPI_Master_Transfer8_TxRx(INSTRUCTION_WRITE);
    SPI_Master_Transfer8_TxRx(reg);
    for (uint8_t i=0; i<n; i++) {
        SPI_Master_Transfer8_TxRx(values[i]);
    }
    SPI_Master_EndTransmission(ON(PIN_RF24_CSN));
}

void _MCP2515_modifyRegister(const MCP2515_REGISTER reg, const uint8_t mask, const uint8_t data)
{
    SPI_Master_StartTransmission(__SPI_REG_MCP2515, OFF(PIN_RF24_CSN));
    SPI_Master_Transfer8_TxRx(INSTRUCTION_BITMOD);
    SPI_Master_Transfer8_TxRx(reg);
    SPI_Master_Transfer8_TxRx(mask);
    SPI_Master_Transfer8_TxRx(data);
    SPI_Master_EndTransmission(ON(PIN_RF24_CSN));
}

uint8_t MCP2515_getStatus(void)
{
    SPI_Master_StartTransmission(__SPI_REG_MCP2515, OFF(PIN_RF24_CSN));
    SPI_Master_Transfer8_TxRx(INSTRUCTION_READ_STATUS);
    uint8_t i = SPI_Master_Transfer8_TxRx(0x00);
    SPI_Master_EndTransmission(ON(PIN_RF24_CSN));

    return i;
}

MCP2515_ERROR MCP2515_setConfigMode()
{
    return _MCP2515_setMode(CANCTRL_REQOP_CONFIG);
}

MCP2515_ERROR MCP2515_setListenOnlyMode()
{
    return _MCP2515_setMode(CANCTRL_REQOP_LISTENONLY);
}

MCP2515_ERROR MCP2515_setSleepMode()
{
    return _MCP2515_setMode(CANCTRL_REQOP_SLEEP);
}

MCP2515_ERROR MCP2515_setLoopbackMode()
{
    return _MCP2515_setMode(CANCTRL_REQOP_LOOPBACK);
}

MCP2515_ERROR MCP2515_setNormalMode()
{
    return _MCP2515_setMode(CANCTRL_REQOP_NORMAL);
}

MCP2515_ERROR _MCP2515_setMode(const MCP2515_CANCTRL_REQOP_MODE mode)
{
    _MCP2515_modifyRegister(MCP_CANCTRL, CANCTRL_REQOP, mode);

    uint32_t endTime = millis() + 10;
    uint8_t modeMatch = 0;
    while (millis() < endTime) {
        uint8_t newmode = _MCP2515_readRegister(MCP_CANSTAT);
        newmode &= CANSTAT_OPMOD;

        modeMatch = newmode == mode;

        if (modeMatch) {
            break;
        }
    }

    return modeMatch ? ERROR_OK : ERROR_FAIL;

}

// MCP2515_ERROR MCP2515_setBitrate(const MCP2515_CAN_SPEED canSpeed)
// {
//     return MCP2515_setBitrateCLK(canSpeed, MCP_16MHZ);
// }

MCP2515_ERROR MCP2515_setBitrate(const MCP2515_CAN_SPEED canSpeed/*, MCP2515_CAN_CLOCK canClock*/)
{
    MCP2515_ERROR error = MCP2515_setConfigMode();
    if (error != ERROR_OK) {
        return error;
    }

    uint8_t set, cfg1, cfg2, cfg3;
    set = 1;

#if defined (MCP2515_CAN_CLOCK_8MHZ)
        switch (canSpeed) {

            case (CAN_5KBPS):                                               //   5KBPS
            cfg1 = MCP_8MHz_5kBPS_CFG1;
            cfg2 = MCP_8MHz_5kBPS_CFG2;
            cfg3 = MCP_8MHz_5kBPS_CFG3;
            break;

            case (CAN_10KBPS):                                              //  10KBPS
            cfg1 = MCP_8MHz_10kBPS_CFG1;
            cfg2 = MCP_8MHz_10kBPS_CFG2;
            cfg3 = MCP_8MHz_10kBPS_CFG3;
            break;

            case (CAN_20KBPS):                                              //  20KBPS
            cfg1 = MCP_8MHz_20kBPS_CFG1;
            cfg2 = MCP_8MHz_20kBPS_CFG2;
            cfg3 = MCP_8MHz_20kBPS_CFG3;
            break;

            case (CAN_31K25BPS):                                            //  31.25KBPS
            cfg1 = MCP_8MHz_31k25BPS_CFG1;
            cfg2 = MCP_8MHz_31k25BPS_CFG2;
            cfg3 = MCP_8MHz_31k25BPS_CFG3;
            break;

            case (CAN_33KBPS):                                              //  33.333KBPS
            cfg1 = MCP_8MHz_33k3BPS_CFG1;
            cfg2 = MCP_8MHz_33k3BPS_CFG2;
            cfg3 = MCP_8MHz_33k3BPS_CFG3;
            break;

            case (CAN_40KBPS):                                              //  40Kbps
            cfg1 = MCP_8MHz_40kBPS_CFG1;
            cfg2 = MCP_8MHz_40kBPS_CFG2;
            cfg3 = MCP_8MHz_40kBPS_CFG3;
            break;

            case (CAN_50KBPS):                                              //  50Kbps
            cfg1 = MCP_8MHz_50kBPS_CFG1;
            cfg2 = MCP_8MHz_50kBPS_CFG2;
            cfg3 = MCP_8MHz_50kBPS_CFG3;
            break;

            case (CAN_80KBPS):                                              //  80Kbps
            cfg1 = MCP_8MHz_80kBPS_CFG1;
            cfg2 = MCP_8MHz_80kBPS_CFG2;
            cfg3 = MCP_8MHz_80kBPS_CFG3;
            break;

            case (CAN_100KBPS):                                             // 100Kbps
            cfg1 = MCP_8MHz_100kBPS_CFG1;
            cfg2 = MCP_8MHz_100kBPS_CFG2;
            cfg3 = MCP_8MHz_100kBPS_CFG3;
            break;

            case (CAN_125KBPS):                                             // 125Kbps
            cfg1 = MCP_8MHz_125kBPS_CFG1;
            cfg2 = MCP_8MHz_125kBPS_CFG2;
            cfg3 = MCP_8MHz_125kBPS_CFG3;
            break;

            case (CAN_200KBPS):                                             // 200Kbps
            cfg1 = MCP_8MHz_200kBPS_CFG1;
            cfg2 = MCP_8MHz_200kBPS_CFG2;
            cfg3 = MCP_8MHz_200kBPS_CFG3;
            break;

            case (CAN_250KBPS):                                             // 250Kbps
            cfg1 = MCP_8MHz_250kBPS_CFG1;
            cfg2 = MCP_8MHz_250kBPS_CFG2;
            cfg3 = MCP_8MHz_250kBPS_CFG3;
            break;

            case (CAN_500KBPS):                                             // 500Kbps
            cfg1 = MCP_8MHz_500kBPS_CFG1;
            cfg2 = MCP_8MHz_500kBPS_CFG2;
            cfg3 = MCP_8MHz_500kBPS_CFG3;
            break;

            case (CAN_1000KBPS):                                            //   1Mbps
            cfg1 = MCP_8MHz_1000kBPS_CFG1;
            cfg2 = MCP_8MHz_1000kBPS_CFG2;
            cfg3 = MCP_8MHz_1000kBPS_CFG3;
            break;

            default:
            set = 0;
            break;
        }

#elif defined (MCP2515_CAN_CLOCK_16MHZ)

        switch (canSpeed) {

            case (CAN_5KBPS):                                               //   5Kbps
            cfg1 = MCP_16MHz_5kBPS_CFG1;
            cfg2 = MCP_16MHz_5kBPS_CFG2;
            cfg3 = MCP_16MHz_5kBPS_CFG3;
            break;

            case (CAN_10KBPS):                                              //  10Kbps
            cfg1 = MCP_16MHz_10kBPS_CFG1;
            cfg2 = MCP_16MHz_10kBPS_CFG2;
            cfg3 = MCP_16MHz_10kBPS_CFG3;
            break;

            case (CAN_20KBPS):                                              //  20Kbps
            cfg1 = MCP_16MHz_20kBPS_CFG1;
            cfg2 = MCP_16MHz_20kBPS_CFG2;
            cfg3 = MCP_16MHz_20kBPS_CFG3;
            break;

            case (CAN_33KBPS):                                              //  33.333Kbps
            cfg1 = MCP_16MHz_33k3BPS_CFG1;
            cfg2 = MCP_16MHz_33k3BPS_CFG2;
            cfg3 = MCP_16MHz_33k3BPS_CFG3;
            break;

            case (CAN_40KBPS):                                              //  40Kbps
            cfg1 = MCP_16MHz_40kBPS_CFG1;
            cfg2 = MCP_16MHz_40kBPS_CFG2;
            cfg3 = MCP_16MHz_40kBPS_CFG3;
            break;

            case (CAN_50KBPS):                                              //  50Kbps
            cfg1 = MCP_16MHz_50kBPS_CFG1;
            cfg2 = MCP_16MHz_50kBPS_CFG2;
            cfg3 = MCP_16MHz_50kBPS_CFG3;
            break;

            case (CAN_80KBPS):                                              //  80Kbps
            cfg1 = MCP_16MHz_80kBPS_CFG1;
            cfg2 = MCP_16MHz_80kBPS_CFG2;
            cfg3 = MCP_16MHz_80kBPS_CFG3;
            break;

            case (CAN_83K3BPS):                                             //  83.333Kbps
            cfg1 = MCP_16MHz_83k3BPS_CFG1;
            cfg2 = MCP_16MHz_83k3BPS_CFG2;
            cfg3 = MCP_16MHz_83k3BPS_CFG3;
            break;

            case (CAN_100KBPS):                                             // 100Kbps
            cfg1 = MCP_16MHz_100kBPS_CFG1;
            cfg2 = MCP_16MHz_100kBPS_CFG2;
            cfg3 = MCP_16MHz_100kBPS_CFG3;
            break;

            case (CAN_125KBPS):                                             // 125Kbps
            cfg1 = MCP_16MHz_125kBPS_CFG1;
            cfg2 = MCP_16MHz_125kBPS_CFG2;
            cfg3 = MCP_16MHz_125kBPS_CFG3;
            break;

            case (CAN_200KBPS):                                             // 200Kbps
            cfg1 = MCP_16MHz_200kBPS_CFG1;
            cfg2 = MCP_16MHz_200kBPS_CFG2;
            cfg3 = MCP_16MHz_200kBPS_CFG3;
            break;

            case (CAN_250KBPS):                                             // 250Kbps
            cfg1 = MCP_16MHz_250kBPS_CFG1;
            cfg2 = MCP_16MHz_250kBPS_CFG2;
            cfg3 = MCP_16MHz_250kBPS_CFG3;
            break;

            case (CAN_500KBPS):                                             // 500Kbps
            cfg1 = MCP_16MHz_500kBPS_CFG1;
            cfg2 = MCP_16MHz_500kBPS_CFG2;
            cfg3 = MCP_16MHz_500kBPS_CFG3;
            break;

            case (CAN_1000KBPS):                                            //   1Mbps
            cfg1 = MCP_16MHz_1000kBPS_CFG1;
            cfg2 = MCP_16MHz_1000kBPS_CFG2;
            cfg3 = MCP_16MHz_1000kBPS_CFG3;
            break;

            default:
            set = 0;
            break;
        }

#elif defined (MCP2515_CAN_CLOCK_20MHZ)

        switch (canSpeed) {

            case (CAN_33KBPS):                                              //  33.333Kbps
            cfg1 = MCP_20MHz_33k3BPS_CFG1;
            cfg2 = MCP_20MHz_33k3BPS_CFG2;
            cfg3 = MCP_20MHz_33k3BPS_CFG3;
            break;

            case (CAN_40KBPS):                                              //  40Kbps
            cfg1 = MCP_20MHz_40kBPS_CFG1;
            cfg2 = MCP_20MHz_40kBPS_CFG2;
            cfg3 = MCP_20MHz_40kBPS_CFG3;
            break;

            case (CAN_50KBPS):                                              //  50Kbps
            cfg1 = MCP_20MHz_50kBPS_CFG1;
            cfg2 = MCP_20MHz_50kBPS_CFG2;
            cfg3 = MCP_20MHz_50kBPS_CFG3;
            break;

            case (CAN_80KBPS):                                              //  80Kbps
            cfg1 = MCP_20MHz_80kBPS_CFG1;
            cfg2 = MCP_20MHz_80kBPS_CFG2;
            cfg3 = MCP_20MHz_80kBPS_CFG3;
            break;

            case (CAN_83K3BPS):                                             //  83.333Kbps
            cfg1 = MCP_20MHz_83k3BPS_CFG1;
            cfg2 = MCP_20MHz_83k3BPS_CFG2;
            cfg3 = MCP_20MHz_83k3BPS_CFG3;
            break;

            case (CAN_100KBPS):                                             // 100Kbps
            cfg1 = MCP_20MHz_100kBPS_CFG1;
            cfg2 = MCP_20MHz_100kBPS_CFG2;
            cfg3 = MCP_20MHz_100kBPS_CFG3;
            break;

            case (CAN_125KBPS):                                             // 125Kbps
            cfg1 = MCP_20MHz_125kBPS_CFG1;
            cfg2 = MCP_20MHz_125kBPS_CFG2;
            cfg3 = MCP_20MHz_125kBPS_CFG3;
            break;

            case (CAN_200KBPS):                                             // 200Kbps
            cfg1 = MCP_20MHz_200kBPS_CFG1;
            cfg2 = MCP_20MHz_200kBPS_CFG2;
            cfg3 = MCP_20MHz_200kBPS_CFG3;
            break;

            case (CAN_250KBPS):                                             // 250Kbps
            cfg1 = MCP_20MHz_250kBPS_CFG1;
            cfg2 = MCP_20MHz_250kBPS_CFG2;
            cfg3 = MCP_20MHz_250kBPS_CFG3;
            break;

            case (CAN_500KBPS):                                             // 500Kbps
            cfg1 = MCP_20MHz_500kBPS_CFG1;
            cfg2 = MCP_20MHz_500kBPS_CFG2;
            cfg3 = MCP_20MHz_500kBPS_CFG3;
            break;

            case (CAN_1000KBPS):                                            //   1Mbps
            cfg1 = MCP_20MHz_1000kBPS_CFG1;
            cfg2 = MCP_20MHz_1000kBPS_CFG2;
            cfg3 = MCP_20MHz_1000kBPS_CFG3;
            break;

            default:
            set = 0;
            break;
        }

#endif

    if (set) {
        _MCP2515_setRegister(MCP_CNF1, cfg1);
        _MCP2515_setRegister(MCP_CNF2, cfg2);
        _MCP2515_setRegister(MCP_CNF3, cfg3);
        return ERROR_OK;
    }
    else {
        return ERROR_FAIL;
    }
}

MCP2515_ERROR MCP2515_setClkOut(const MCP2515_CAN_CLKOUT divisor)
{
    if (divisor == CLKOUT_DISABLE) {
	/* Turn off CLKEN */
	_MCP2515_modifyRegister(MCP_CANCTRL, CANCTRL_CLKEN, 0x00);

	/* Turn on CLKOUT for SOF */
	_MCP2515_modifyRegister(MCP_CNF3, CNF3_SOF, CNF3_SOF);
        return ERROR_OK;
    }

    /* Set the prescaler (CLKPRE) */
    _MCP2515_modifyRegister(MCP_CANCTRL, CANCTRL_CLKPRE, divisor);

    /* Turn on CLKEN */
    _MCP2515_modifyRegister(MCP_CANCTRL, CANCTRL_CLKEN, CANCTRL_CLKEN);

    /* Turn off CLKOUT for SOF */
    _MCP2515_modifyRegister(MCP_CNF3, CNF3_SOF, 0x00);
    return ERROR_OK;
}

void MCP2515_prepareId(uint8_t *buffer, const uint8_t ext, const uint32_t id)
{
    uint16_t canid = (uint16_t)(id & 0x0FFFF);

    if (ext) {
        buffer[MCP_EID0] = (uint8_t) (canid & 0xFF);
        buffer[MCP_EID8] = (uint8_t) (canid >> 8);
        canid = (uint16_t)(id >> 16);
        buffer[MCP_SIDL] = (uint8_t) (canid & 0x03);
        buffer[MCP_SIDL] += (uint8_t) ((canid & 0x1C) << 3);
        buffer[MCP_SIDL] |= TXB_EXIDE_MASK;
        buffer[MCP_SIDH] = (uint8_t) (canid >> 5);
    } else {
        buffer[MCP_SIDH] = (uint8_t) (canid >> 3);
        buffer[MCP_SIDL] = (uint8_t) ((canid & 0x07 ) << 5);
        buffer[MCP_EID0] = 0;
        buffer[MCP_EID8] = 0;
    }
}

MCP2515_ERROR MCP2515_setFilterMask(const MCP2515_MASK mask, const uint8_t ext, const uint32_t ulData)
{
    MCP2515_ERROR res = MCP2515_setConfigMode();
    if (res != ERROR_OK) {
        return res;
    }

    uint8_t tbufdata[4];
    MCP2515_prepareId(tbufdata, ext, ulData);

    MCP2515_REGISTER reg;
    switch (mask) {
        case MASK0: reg = MCP_RXM0SIDH; break;
        case MASK1: reg = MCP_RXM1SIDH; break;
        default:
            return ERROR_FAIL;
    }

    _MCP2515_setRegistersN(reg, tbufdata, 4);

    return ERROR_OK;
}

MCP2515_ERROR MCP2515_setFilter(const MCP2515_RXF num, const uint8_t ext, const uint32_t ulData)
{
    MCP2515_ERROR res = MCP2515_setConfigMode();
    if (res != ERROR_OK) {
        return res;
    }

    MCP2515_REGISTER reg;

    switch (num) {
        case RXF0: reg = MCP_RXF0SIDH; break;
        case RXF1: reg = MCP_RXF1SIDH; break;
        case RXF2: reg = MCP_RXF2SIDH; break;
        case RXF3: reg = MCP_RXF3SIDH; break;
        case RXF4: reg = MCP_RXF4SIDH; break;
        case RXF5: reg = MCP_RXF5SIDH; break;
        default:
            return ERROR_FAIL;
    }

    uint8_t tbufdata[4];
    MCP2515_prepareId(tbufdata, ext, ulData);
    _MCP2515_setRegistersN(reg, tbufdata, 4);

    return ERROR_OK;
}

MCP2515_ERROR MCP2515_sendMessageTxBN(const MCP2515_TXBn txbn, const struct can_frame *frame)
{
    if (frame->can_dlc > CAN_MAX_DLEN) {
        return ERROR_FAILTX;
    }

    const MCP2515_TXBn_REGS *txbuf = &MCP2515_TXB[txbn];

    uint8_t data[13];

    uint8_t ext = !!(frame->can_id & CAN_EFF_FLAG);
    uint8_t rtr = !!(frame->can_id & CAN_RTR_FLAG);
    uint32_t id = (frame->can_id & (ext ? CAN_EFF_MASK : CAN_SFF_MASK));

    MCP2515_prepareId(data, ext, id);

    data[MCP_DLC] = rtr ? (frame->can_dlc | RTR_MASK) : frame->can_dlc;

    memcpy(&data[MCP_DATA], frame->data, frame->can_dlc);

    _MCP2515_setRegistersN(txbuf->SIDH, data, 5 + frame->can_dlc);

    _MCP2515_modifyRegister(txbuf->CTRL, TXB_TXREQ, TXB_TXREQ);

    uint8_t ctrl = _MCP2515_readRegister(txbuf->CTRL);
    if ((ctrl & (TXB_ABTF | TXB_MLOA | TXB_TXERR)) != 0) {
        return ERROR_FAILTX;
    }
    return ERROR_OK;
}

MCP2515_ERROR MCP2515_sendMessage(const struct can_frame *frame)
{
    if (frame->can_dlc > CAN_MAX_DLEN) {
        return ERROR_FAILTX;
    }

    MCP2515_TXBn txBuffers[MCP2515_N_TXBUFFERS] = {TXB0, TXB1, TXB2};

    for (int i=0; i < MCP2515_N_TXBUFFERS; ++i) {
        const MCP2515_TXBn_REGS *txbuf = &MCP2515_TXB[txBuffers[i]];
        uint8_t ctrlval = _MCP2515_readRegister(txbuf->CTRL);
        if ( (ctrlval & TXB_TXREQ) == 0 ) {
            return MCP2515_sendMessageTxBN(txBuffers[i], frame);
        }
    }

    return ERROR_ALLTXBUSY;
}

MCP2515_ERROR MCP2515_readMessageRxBN(const MCP2515_RXBn rxbn, struct can_frame *frame)
{
    const MCP2515_RXBn_REGS *rxb = &MCP2515_RXB[rxbn];

    uint8_t tbufdata[5];

    _MCP2515_readRegisters(rxb->SIDH, tbufdata, 5);

    uint32_t id = (tbufdata[MCP_SIDH]<<3) + (tbufdata[MCP_SIDL]>>5);

    if ( (tbufdata[MCP_SIDL] & TXB_EXIDE_MASK) ==  TXB_EXIDE_MASK ) {
        id = (id<<2) + (tbufdata[MCP_SIDL] & 0x03);
        id = (id<<8) + tbufdata[MCP_EID8];
        id = (id<<8) + tbufdata[MCP_EID0];
        id |= CAN_EFF_FLAG;
    }

    uint8_t dlc = (tbufdata[MCP_DLC] & DLC_MASK);
    if (dlc > CAN_MAX_DLEN) {
        return ERROR_FAIL;
    }

    uint8_t ctrl = _MCP2515_readRegister(rxb->CTRL);
    if (ctrl & RXBnCTRL_RTR) {
        id |= CAN_RTR_FLAG;
    }

    frame->can_id = id;
    frame->can_dlc = dlc;

    _MCP2515_readRegisters(rxb->DATA, frame->data, dlc);

    _MCP2515_modifyRegister(MCP_CANINTF, rxb->CANINTF_RXnIF, 0);

    return ERROR_OK;
}

MCP2515_ERROR MCP2515_readMessage(struct can_frame *frame)
{
    MCP2515_ERROR rc;
    uint8_t stat = MCP2515_getStatus();

    if ( stat & STAT_RX0IF ) {
        rc = MCP2515_readMessageRxBN(RXB0, frame);
    } else if ( stat & STAT_RX1IF ) {
        rc = MCP2515_readMessageRxBN(RXB1, frame);
    } else {
        rc = ERROR_NOMSG;
    }

    return rc;
}

uint8_t MCP2515_checkReceive(void)
{
    uint8_t res = MCP2515_getStatus();
    if ( res & STAT_RXIF_MASK ) {
        return 1;
    } else {
        return 0;
    }
}

uint8_t MCP2515_checkError(void)
{
    uint8_t eflg = MCP2515_getErrorFlags();

    if ( eflg & EFLG_ERRORMASK ) {
        return 1;
    } else {
        return 0;
    }
}

uint8_t MCP2515_getErrorFlags(void)
{
    return _MCP2515_readRegister(MCP_EFLG);
}

void MCP2515_clearRXnOVRFlags(void)
{
	_MCP2515_modifyRegister(MCP_EFLG, EFLG_RX0OVR | EFLG_RX1OVR, 0);
}

uint8_t MCP2515_getInterrupts(void)
{
    return _MCP2515_readRegister(MCP_CANINTF);
}

void MCP2515_clearInterrupts(void)
{
    _MCP2515_setRegister(MCP_CANINTF, 0);
}

uint8_t MCP2515_getInterruptMask(void)
{
    return _MCP2515_readRegister(MCP_CANINTE);
}

void MCP2515_clearTXInterrupts(void)
{
    _MCP2515_modifyRegister(MCP_CANINTF, (CANINTF_TX0IF | CANINTF_TX1IF | CANINTF_TX2IF), 0);
}

void MCP2515_clearRXnOVR(void)
{
	uint8_t eflg = MCP2515_getErrorFlags();
	if (eflg != 0) {
		MCP2515_clearRXnOVRFlags();
		MCP2515_clearInterrupts();
		//_MCP2515_modifyRegister(MCP_CANINTF, CANINTF_ERRIF, 0);
	}
	
}

void MCP2515_clearMERR()
{
	//_MCP2515_modifyRegister(MCP_EFLG, EFLG_RX0OVR | EFLG_RX1OVR, 0);
	//MCP2515_clearInterrupts();
	_MCP2515_modifyRegister(MCP_CANINTF, CANINTF_MERRF, 0);
}

void MCP2515_clearERRIF()
{
    //_MCP2515_modifyRegister(MCP_EFLG, EFLG_RX0OVR | EFLG_RX1OVR, 0);
    //MCP2515_clearInterrupts();
    _MCP2515_modifyRegister(MCP_CANINTF, CANINTF_ERRIF, 0);
}

uint8_t MCP2515_errorCountRX(void)
{
    return _MCP2515_readRegister(MCP_REC);
}

uint8_t MCP2515_errorCountTX(void)
{
    return _MCP2515_readRegister(MCP_TEC);
}
