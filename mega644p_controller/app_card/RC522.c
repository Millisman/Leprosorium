// based on https://github.com/AlekseySedyshev

#include "RC522.h"
#include <stdio.h>
#include <string.h>

static MI_STATUS _MI_status;
static uint16_t  _MI_ulen16;
static uint8_t   _MI_buff[18];

uint8_t *MFRC522_GetBuff() {
    return _MI_buff;
}

MI_STATUS MFRC522_Check() {
    _MI_status = MFRC522_Request(PICC_REQIDL); // Find cards, return card type
    if (_MI_status == MI_OK) {
        _MI_status = MFRC522_Anticoll(); } // Card detected. Anti-collision,
                                                        // return card serial number 4 bytes
    MFRC522_Halt();                                     // Command card into hibernation
    return _MI_status;
}

void MFRC522_SetBitMask(uint8_t reg, uint8_t mask) {
    MFRC522_WriteRegister(reg, MFRC522_ReadRegister(reg) | mask);
}

void MFRC522_ClearBitMask(uint8_t reg, uint8_t mask){
    MFRC522_WriteRegister(reg, MFRC522_ReadRegister(reg) & (~mask));
}

MI_STATUS MFRC522_Request(uint8_t reqMode) {
    MFRC522_WriteRegister(MFRC522_REG_BIT_FRAMING, 0x07); // TxLastBists = BitFramingReg[2..0]
    _MI_buff[0] = reqMode;
    _MI_status = MFRC522_ToCard(PCD_TRANSCEIVE, _MI_buff, 1, _MI_buff, &_MI_ulen16);
    if ((_MI_status != MI_OK) || (_MI_ulen16 != 0x10)) _MI_status = MI_ERR;
    return _MI_status;
}

MI_STATUS MFRC522_ToCard(uint8_t    cmd,
                         uint8_t*   inData,     uint8_t    inLen,
                         uint8_t*   outData,    uint16_t*  outLen
                         )
{
    _MI_status = MI_ERR;
    uint8_t irqEn = 0x00;
    uint8_t waitIRq = 0x00;
    uint8_t lastBits;
    uint8_t n;
    uint16_t i;

    switch (cmd) {
        case PCD_AUTHENT: {
            irqEn = 0x12;
            waitIRq = 0x10;
            break;
        }
        case PCD_TRANSCEIVE: {
            irqEn = 0x77;
            waitIRq = 0x30;
            break;
        }
        default:
            break;
    }

    MFRC522_WriteRegister(MFRC522_REG_COMM_IE_N, irqEn | 0x80);
    MFRC522_ClearBitMask(MFRC522_REG_COMM_IRQ, 0x80);
    MFRC522_SetBitMask(MFRC522_REG_FIFO_LEVEL, 0x80);
    MFRC522_WriteRegister(MFRC522_REG_COMMAND, PCD_IDLE);

    // Writing data to the FIFO
    for (i = 0; i < inLen; i++) MFRC522_WriteRegister(MFRC522_REG_FIFO_DATA, inData[i]);

    // Execute the command
    MFRC522_WriteRegister(MFRC522_REG_COMMAND, cmd);
    if (cmd == PCD_TRANSCEIVE) MFRC522_SetBitMask(MFRC522_REG_BIT_FRAMING, 0x80); // StartSend=1,transmission of data starts

    // Waiting to receive data to complete
    i = 2000;    // i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms
    do {
        // CommIrqReg[7..0]
        // Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
        n = MFRC522_ReadRegister(MFRC522_REG_COMM_IRQ);
        i--;
    } while ((i!=0) && !(n&0x01) && !(n&waitIRq));

    MFRC522_ClearBitMask(MFRC522_REG_BIT_FRAMING, 0x80); // StartSend=0

    if (i != 0)  {
        if (!(MFRC522_ReadRegister(MFRC522_REG_ERROR) & 0x1B)) {
            _MI_status = MI_OK;
            if (n & irqEn & 0x01) _MI_status = MI_NOTAGERR;
            if (cmd == PCD_TRANSCEIVE) {
                n = MFRC522_ReadRegister(MFRC522_REG_FIFO_LEVEL);
                lastBits = MFRC522_ReadRegister(MFRC522_REG_CONTROL) & 0x07;
                if (lastBits) {
                    *outLen = (n-1) * 8 + lastBits;
                } else {
                    *outLen = n * 8;
                }
                if (n == 0) n = 1;
                if (n > MFRC522_MAX_LEN) n = MFRC522_MAX_LEN;
                for (i = 0; i < n; i++) {
                    outData[i] = MFRC522_ReadRegister(MFRC522_REG_FIFO_DATA); // Reading the received data in FIFO
                }
            }
        } else _MI_status = MI_ERR;
    }
    return _MI_status;
}

MI_STATUS MFRC522_Anticoll() {
    MFRC522_WriteRegister(MFRC522_REG_BIT_FRAMING, 0x00); // TxLastBists = BitFramingReg[2..0]
    memset((void *)&_MI_buff, 0, sizeof(_MI_buff));
    _MI_buff[0] = PICC_ANTICOLL;
    _MI_buff[1] = 0x20;
    _MI_status = MFRC522_ToCard(PCD_TRANSCEIVE, _MI_buff, 2, _MI_buff, &_MI_ulen16);
    if (_MI_status == MI_OK) {
        // Check card serial number
        // for (i = 0; i < maxLen; i++) serNumCheck ^= serNum[i];
        //if (serNumCheck != serNum[i]) status = MI_ERR;
    }
    return _MI_status;
}

void MFRC522_CalculateCRC(uint8_t*  pIndata, uint8_t len, uint8_t* pOutData) {
    uint8_t i, n;
    MFRC522_ClearBitMask(MFRC522_REG_DIV_IRQ,  0x04); // CRCIrq = 0
    MFRC522_SetBitMask(MFRC522_REG_FIFO_LEVEL, 0x80); // Clear the FIFO pointer
    // Write_MFRC522(CommandReg, PCD_IDLE);
    // Writing data to the FIFO
    for (i = 0; i < len; i++) MFRC522_WriteRegister(MFRC522_REG_FIFO_DATA, *(pIndata+i));
    MFRC522_WriteRegister(MFRC522_REG_COMMAND, PCD_CALCCRC);
    i = 0xFF; // Wait CRC calculation is complete
    do {
        n = MFRC522_ReadRegister(MFRC522_REG_DIV_IRQ);
        i--;
    } while ((i != 0) && !(n & 0x04)); // CRCIrq = 1
    // Read CRC calculation result
    pOutData[0] = MFRC522_ReadRegister(MFRC522_REG_CRC_RESULT_L);
    pOutData[1] = MFRC522_ReadRegister(MFRC522_REG_CRC_RESULT_M);
}

uint8_t MFRC522_SelectTag(uint8_t* serNum) {
    _MI_buff[0] = PICC_SElECTTAG;
    _MI_buff[1] = 0x70;
    for (uint8_t i = 0; i < 5; i++) _MI_buff[i+2] = *(serNum+i);
    MFRC522_CalculateCRC(_MI_buff, 7, &_MI_buff[7]); //??
    _MI_status = MFRC522_ToCard(PCD_TRANSCEIVE, _MI_buff, 9, _MI_buff, &_MI_ulen16);
    if ((_MI_status == MI_OK) && (_MI_ulen16 == 0x18)) { return _MI_buff[0]; }
    return 0;
}

MI_STATUS MFRC522_Auth(uint8_t authMode, uint8_t BlockAddr, uint8_t* Sectorkey, uint8_t* serNum) {
    // Verify the command block address + sector + password + card serial number
    _MI_buff[0] = authMode;
    _MI_buff[1] = BlockAddr;
    for (uint8_t i = 0; i < 6; i++) _MI_buff[i+2] = *(Sectorkey + i);
    for (uint8_t i = 0; i < 4; i++) _MI_buff[i+8] = *(serNum + i);
    _MI_status = MFRC522_ToCard(PCD_AUTHENT, _MI_buff, 12, _MI_buff, &_MI_ulen16);
    if ((_MI_status != MI_OK) || (!(MFRC522_ReadRegister(MFRC522_REG_STATUS2) & 0x08))) _MI_status = MI_ERR;
    return _MI_status;
}

MI_STATUS MFRC522_Read(uint8_t blockAddr, uint8_t* recvData) {
    recvData[0] = PICC_READ;
    recvData[1] = blockAddr;
    MFRC522_CalculateCRC(recvData,2, &recvData[2]);
    _MI_status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &_MI_ulen16);
    if ((_MI_status != MI_OK) || (_MI_ulen16 != 0x90)) _MI_status = MI_ERR;
    return _MI_status;
}

MI_STATUS MFRC522_Write(uint8_t blockAddr, uint8_t* writeData) {
    _MI_buff[0] = PICC_WRITE;
    _MI_buff[1] = blockAddr;
    MFRC522_CalculateCRC(_MI_buff, 2, &_MI_buff[2]);
    _MI_status = MFRC522_ToCard(PCD_TRANSCEIVE, _MI_buff, 4, _MI_buff, &_MI_ulen16);
    if ((_MI_status != MI_OK) || (_MI_ulen16 != 4) || ((_MI_buff[0] & 0x0F) != 0x0A)) _MI_status = MI_ERR;
    if (_MI_status == MI_OK) {
        // Data to the FIFO write 16Byte
        for (uint8_t i = 0; i < 16; i++) _MI_buff[i] = *(writeData+i);
        MFRC522_CalculateCRC(_MI_buff, 16, &_MI_buff[16]);
        _MI_status = MFRC522_ToCard(PCD_TRANSCEIVE, _MI_buff, 18, _MI_buff, &_MI_ulen16);
        if ((_MI_status != MI_OK) || (_MI_ulen16 != 4) || ((_MI_buff[0] & 0x0F) != 0x0A)) _MI_status = MI_ERR;
    }
    return _MI_status;
}

void MFRC522_Reset(void) {
    MFRC522_WriteRegister(MFRC522_REG_COMMAND, PCD_RESETPHASE);
}

void MFRC522_AntennaOn(void) {
    if (!(MFRC522_ReadRegister(MFRC522_REG_TX_CONTROL) & 0x03)) {
        MFRC522_SetBitMask(MFRC522_REG_TX_CONTROL, 0x03);
    }
}

void MFRC522_AntennaOff(void) {
    MFRC522_ClearBitMask(MFRC522_REG_TX_CONTROL, 0x03);
}

void MFRC522_Halt(void) {
    _MI_buff[0] = PICC_HALT;
    _MI_buff[1] = 0;
    MFRC522_CalculateCRC(_MI_buff, 2, &_MI_buff[2]);
    MFRC522_ToCard(PCD_TRANSCEIVE, _MI_buff, 4, _MI_buff, &_MI_ulen16);
}


