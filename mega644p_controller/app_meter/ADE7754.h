// #pragma once
// 
// #include <stdint.h>
// #include <stdbool.h>
// #include <util/delay.h>
// 
#define ADE7754_STARTUP_DELAY   1
#define ADE7754_READ_REG(a)    a
#define ADE7754_WRITE_REG(a) ((a) | 0x80)
// 
// #define ADE7754_SPI_SLOW     300000UL
// #define ADE7754_SPI_BURST   1000000UL
// #define ADE7754_SPI_FAST    2000000UL
// 
// #define ADE7754_REG_AENERGY    0x01
// #define ADE7754_REG_RAENERGY   0x02
// #define ADE7754_REG_LAENERGY   0x03
// #define ADE7754_REG_VAENERGY   0x04
// #define ADE7754_REG_RVAENERGY  0x05
// #define ADE7754_REG_LVAENERGY  0x06
// #define ADE7754_REG_PERIOD     0x07
// #define ADE7754_REG_TEMP       0x08
// #define ADE7754_REG_WFORM      0x09
// #define ADE7754_REG_OPMODE     0x0A
// #define ADE7754_REG_MMODE      0x0B
// #define ADE7754_REG_WAVMODE    0x0C
// #define ADE7754_REG_WATMODE    0x0D
// #define ADE7754_REG_VAMODE     0x0E
// #define ADE7754_REG_IRQEN      0x0F
// #define ADE7754_REG_STATUS     0x10
// #define ADE7754_REG_RSTATUS    0x11
// #define ADE7754_REG_ZXTOUT     0x12
// #define ADE7754_REG_LINCYC     0x13
// #define ADE7754_REG_SAGCYC     0x14
// #define ADE7754_REG_SAGLVL     0x15
// #define ADE7754_REG_VPEAK      0x16
// #define ADE7754_REG_IPEAK      0x17
// #define ADE7754_REG_GAIN       0x18
// #define ADE7754_REG_AWG        0x19
// #define ADE7754_REG_BWG        0x1A
// #define ADE7754_REG_CWG        0x1B
// #define ADE7754_REG_AVAG       0x1C
// #define ADE7754_REG_BVAG       0x1D
// #define ADE7754_REG_CVAG       0x1E
// #define ADE7754_REG_APHCAL     0x1F
// #define ADE7754_REG_BPHCAL     0x20
// #define ADE7754_REG_CPHCAL     0x21
// #define ADE7754_REG_AAPOS      0x22
// #define ADE7754_REG_BAPOS      0x23
// #define ADE7754_REG_CAPOS      0x24
// #define ADE7754_REG_CFNUM      0x25
// #define ADE7754_REG_CFDEN      0x26
// #define ADE7754_REG_WDIV       0x27
// #define ADE7754_REG_VADIV      0x28
// #define ADE7754_REG_AIRMS      0x29
// #define ADE7754_REG_BIRMS      0x2A
// #define ADE7754_REG_CIRMS      0x2B
// #define ADE7754_REG_AVRMS      0x2C
// #define ADE7754_REG_BVRMS      0x2D
// #define ADE7754_REG_CVRMS      0x2E
// #define ADE7754_REG_AIRMSOS    0x2F
// #define ADE7754_REG_BIRMSOS    0x30
// #define ADE7754_REG_CIRMSOS    0x31
// #define ADE7754_REG_AVRMSOS    0x32
// #define ADE7754_REG_BVRMSOS    0x33
// #define ADE7754_REG_CVRMSOS    0x34
// #define ADE7754_REG_AAPGAIN    0x35
// #define ADE7754_REG_BAPGAIN    0x36
// #define ADE7754_REG_CAPGAIN    0x37
// #define ADE7754_REG_AVGAIN     0x38
// #define ADE7754_REG_BVGAIN     0x39
// #define ADE7754_REG_CVGAIN     0x3A
// #define ADE7754_REG_CHKSUM     0x3E
// #define ADE7754_REG_VERSION    0x3F
// 
// 
// void        ADE7754_spi_write_reg_8(uint8_t reg_address, uint8_t val);
// void        ADE7754_spi_write_reg_16(uint8_t reg_address, uint16_t value);
// uint8_t     ADE7754_spi_read_reg_8(uint8_t reg_address);
// uint16_t    ADE7754_spi_read_reg_16(uint8_t reg_address);
// uint32_t    ADE7754_spi_read_reg_24(uint8_t reg_address);
// 
// void        ADE7754_spi_write_reg_8(uint8_t reg_address, uint8_t val);
// void        ADE7754_spi_write_reg_16(uint8_t reg_address, uint16_t value);
// uint8_t     ADE7754_spi_read_reg_8(uint8_t reg_address);
// uint16_t    ADE7754_spi_read_reg_16(uint8_t reg_address);
// uint32_t    ADE7754_spi_read_reg_24(uint8_t reg_address);
// 
// uint8_t  ADE7754_read_8bit(uint8_t address);
// uint16_t ADE7754_read_16bit(uint8_t address);
// uint32_t ADE7754_read_24bit(uint8_t address);
// void ADE7754_write_8bit(uint8_t address, uint8_t val);
// void ADE7754_write_16bit(uint8_t address, uint16_t val);
// 
// uint32_t    ADE7754_ActiveEnergy();
// uint32_t    ADE7754_ActiveEnergy_Reset();
// uint32_t    ADE7754_LineAccumulationActiveEnergy();
// uint32_t    ADE7754_VAEnergy();
// uint32_t    ADE7754_VAEnergy_Reset();
// uint32_t    ADE7754_ApparentEnergy();
// uint16_t    ADE7754_PeriodLineZC();
// uint8_t     ADE7754_Temperature();
// uint32_t    ADE7754_Waveform();
// uint8_t     ADE7754_OperationalMode_get();
// void        ADE7754_OperationalMode(uint8_t val);
// uint8_t     ADE7754_MeasurementMode_get();
// void        ADE7754_MeasurementMode(uint8_t val);
// uint8_t     ADE7754_WaveformMode_get();
// void        ADE7754_WaveformMode(uint8_t val);
// uint8_t     ADE7754_WatMode_get();
// void        ADE7754_WatMode(uint8_t val);
// uint8_t     ADE7754_VaMode_get();
// void        ADE7754_VaMode(uint8_t val);
// uint16_t    ADE7754_IRQEnable_get();
// void        ADE7754_IRQEnable(uint16_t val);
// uint16_t    ADE7754_IRQStatus_get();
// uint16_t    ADE7754_IRQStatus_Reset_get();
// uint16_t    ADE7754_ZeroCrossTimeout_get();
// void        ADE7754_ZeroCrossTimeout(uint16_t val);
// uint16_t    ADE7754_LineCycle_get();
// void        ADE7754_LineCycle(uint16_t val);
// uint8_t     ADE7754_SAGLineCycle_get();
// void        ADE7754_SAGLineCycle(uint8_t val);
// uint8_t     ADE7754_SAGVoltageLevel_get();
// void        ADE7754_SAGVoltageLevel(uint8_t val);
// uint8_t     ADE7754_VoltagePeakLevel_get();
// void        ADE7754_VoltagePeakLevel(uint8_t val);
// uint8_t     ADE7754_CurrentPeakLevel_get();
// void        ADE7754_CurrentPeakLevel(uint8_t val);
// uint8_t     ADE7754_PGAGain_get();
// void        ADE7754_PGAGain(uint8_t val);
// uint16_t    ADE7754_ActivePowerGainA_get();
// void        ADE7754_ActivePowerGainA(uint16_t val);
// uint16_t    ADE7754_ActivePowerGainB_get();
// void        ADE7754_ActivePowerGainB(uint16_t val);
// uint16_t    ADE7754_ActivePowerGainC_get();
// void        ADE7754_ActivePowerGainC(uint16_t val);
// uint16_t    ADE7754_VAPowerGainA_get();
// void        ADE7754_VAPowerGainA(uint16_t val);
// uint16_t    ADE7754_VAPowerGainB_get();
// void        ADE7754_VAPowerGainB(uint16_t val);
// uint16_t    ADE7754_VAPowerGainC_get();
// void        ADE7754_VAPowerGainC(uint16_t val);
// uint8_t     ADE7754_PhaseCalibrationA_get();
// void        ADE7754_PhaseCalibrationA(uint8_t val);
// uint8_t     ADE7754_PhaseCalibrationB_get();
// void        ADE7754_PhaseCalibrationB(uint8_t val);
// uint8_t     ADE7754_PhaseCalibrationC_get();
// void        ADE7754_PhaseCalibrationC(uint8_t val);
// uint16_t    ADE7754_PowerOffsetCalibrationA_get();
// void        ADE7754_PowerOffsetCalibrationA(uint16_t val);
// uint16_t    ADE7754_PowerOffsetCalibrationB_get();
// void        ADE7754_PowerOffsetCalibrationB(uint16_t val);
// uint16_t    ADE7754_PowerOffsetCalibrationC_get();
// void        ADE7754_PowerOffsetCalibrationC(uint16_t val);
// uint16_t    ADE7754_CF_ScalingNumerator_get();
// void        ADE7754_CF_ScalingNumerator(uint16_t val);
// uint16_t    ADE7754_CF_ScalingDenominator_get();
// void        ADE7754_CF_ScalingDenominator(uint16_t val);
// uint8_t     ADE7754_ActiveEnergyDiv_get();
// void        ADE7754_ActiveEnergyDiv(uint8_t val);
// uint8_t     ADE7754_ApparentEnergyDiv_get();
// void        ADE7754_ApparentEnergyDiv(uint8_t val);
// uint32_t    ADE7754_AIrms_get();
// uint32_t    ADE7754_BIrms_get();
// uint32_t    ADE7754_CIrms_get();
// uint32_t    ADE7754_AVrms_get();
// uint32_t    ADE7754_BVrms_get();
// uint32_t    ADE7754_CVrms_get();
// uint16_t    ADE7754_AIrmsOS_get();
// void        ADE7754_AIrmsOS(uint16_t val);
// uint16_t    ADE7754_BIrmsOS_get();
// void        ADE7754_BIrmsOS(uint16_t val);
// uint16_t    ADE7754_CIrmsOS_get();
// void        ADE7754_CIrmsOS(uint16_t val);
// uint16_t    ADE7754_AVrmsOS_get();
// void        ADE7754_AVrmsOS(uint16_t val);
// uint16_t    ADE7754_BVrmsOS_get();
// void        ADE7754_BVrmsOS(uint16_t val);
// uint16_t    ADE7754_CVrmsOS_get();
// void        ADE7754_CVrmsOS(uint16_t val);
// uint16_t    ADE7754_ActivePowerGainAdjustA_get();
// void        ADE7754_ActivePowerGainAdjustA(uint16_t val);
// uint16_t    ADE7754_ActivePowerGainAdjustB_get();
// void        ADE7754_ActivePowerGainAdjustB(uint16_t val);
// uint16_t    ADE7754_ActivePowerGainAdjustC_get();
// void        ADE7754_ActivePowerGainAdjustC(uint16_t val);
// uint16_t    ADE7754_RMSVoltageGainA_get();
// void        ADE7754_RMSVoltageGainA(uint16_t val);
// uint16_t    ADE7754_RMSVoltageGainB_get();
// void        ADE7754_RMSVoltageGainB(uint16_t val);
// uint16_t    ADE7754_RMSVoltageGainC_get();
// void        ADE7754_RMSVoltageGainC(uint16_t val);
// uint8_t     ADE7754_CheckSum_get();
// uint8_t     ADE7754_Version_get();
// uint8_t     ADE7754_TemperatureOffset_get();
// uint8_t     ADE7754_TemperatureScale_get();
// void        ADE7754_IRQ_Waveform(bool enable);
// void        ADE7754_OperationalMode_ResetChip();
// void        ADE7754_OperationalMode_PoweredDown();
// void        ADE7754_InitialSetup();
// uint16_t    ADE7754_Read_Sampling_Frequency_get();
// void        ADE7754_Write_Sampling_Frequency(uint16_t val);    
