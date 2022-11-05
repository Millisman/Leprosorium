// #include "ADE7754.h"
// 
// // * * * TIMING CHARACTERISTICS * * *
// // Write Timing
// // t1  50 ns (min) CS Falling Edge to First SCLK Falling Edge
// // t2  50 ns (min) SCLK Logic High Pulsewidth
// // t3  50 ns (min) SCLK Logic Low Pulsewidth
// // t4  10 ns (min) Valid Data Setup Time before Falling Edge of SCLK
// // t5   5 ns (min) Data Hold Time after SCLK Falling Edge
// // t6 400 ns (min) Minimum Time between the End of Data Byte Transfers
// // t7  50 ns (min) Minimum Time between Byte Transfers during a Serial Write
// // t8 100 ns (min) CS Hold Time after SCLK Falling Edge
// 
// // Read Timing
// // t9   4 µs (min) Minimum Time between Read Command (i.e., a Write to Communication Register) and Data Read
// // t10 50 ns (min) Minimum Time between Data Byte Transfers during a Multibyte Read
// // t11 30 ns (min) Data Access Time after SCLK Rising Edge following a Write to the
// 
// // Communications Register
// // t12  100 ns (max), 10 ns (min) Bus Relinquish Time after Falling Edge of SCLK
// // t13  100 ns (max), 10 ns (min) Bus Relinquish Time after Rising Edge of CS
// 
//     
// uint8_t ADE7754_read_8bit(uint8_t address) {
//     return ADE7754_spi_read_reg_8(address);
// }
// 
// uint16_t ADE7754_read_16bit(uint8_t address) {
//     return ADE7754_spi_read_reg_16(address);
// }
// 
// uint32_t ADE7754_read_24bit(uint8_t address) {
//     return ADE7754_spi_read_reg_24(address);
// }
// 
// 
// void ADE7754_write_8bit(uint8_t address, uint8_t val) {
//     ADE7754_spi_write_reg_8(address, val);
// }
// 
// void ADE7754_write_16bit(uint8_t address, uint16_t val) {
//     ADE7754_spi_write_reg_16(address, val);
// }
// 
// // R(24)=0 Active Energy Register. Active power is accumulated over time in an internal register.
// // The AENERGY register is a read-only register that reads this
// // internal register and can hold a minimum of 88 seconds of active energy
// // information with full-scale analog inputs before it overflows. See the Energy
// // Calculation section. Bits 7 to 3 of the WATMODE register determine how
// // the active energy is processed from the six analog inputs. See Table XIV
// uint32_t ADE7754_ActiveEnergy() { return ADE7754_read_24bit(ADE7754_REG_AENERGY); }
// 
// // R(24)=0 Same as the AENERGY register, except that the internal register is reset
// // to 0 following a read operation.
// uint32_t ADE7754_ActiveEnergy_Reset() { return ADE7754_read_24bit(ADE7754_REG_RAENERGY); }
// 
// // R(24)=0 Line Accumulation Active Energy Register. The instantaneous active power is
// // accumulated in this read-only register over the LINCYC number of half line
// // cycles. Bits 2 to 0 of the WATMODE register determine how the line accumulation
// // active energy is processed from the six analog inputs. See Table XIV.
// uint32_t ADE7754_LineAccumulationActiveEnergy() { return ADE7754_read_24bit(ADE7754_REG_LAENERGY); }
// 
// // R(24)=0 VA Energy Register. Apparent power is accumulated over time in this
// // read-only register. Bits 7 to 3 of the VAMODE register determine how the
// // apparent energy is processed from the six analog inputs. See Table XV
// uint32_t ADE7754_VAEnergy() { return ADE7754_read_24bit(ADE7754_REG_VAENERGY); }
// 
// // R(24)=0 Same as the VAENERGY register except that the register is reset to 0
// // following a read operation.
// uint32_t ADE7754_VAEnergy_Reset() { return ADE7754_read_24bit(ADE7754_REG_RVAENERGY); }
// 
// // R(24)=0 Apparent Energy Register. The instantaneous apparent power is accumulated in 
// // this read-only register over the LINCYC number of half line
// // cycles. Bits 2 to 0 of the VAMODE register determine how the apparent
// // energy is processed from the six analog inputs. See Table XV.
// uint32_t ADE7754_ApparentEnergy() { return ADE7754_read_24bit(ADE7754_REG_LVAENERGY); }
// 
// // R(15)=0 Period of the line input estimated by zero-crossing processing. Data Bit 0
// // and 1 and 4 to 6 of the MMODE register determine the voltage channel
// // used for period calculation. See Table XII.
// uint16_t ADE7754_PeriodLineZC() { return ADE7754_read_16bit(ADE7754_REG_PERIOD); }
// 
// // R(8)=0 Temperature Register. This register contains the result of the latest
// // temperature conversion. Refer to the Temperature Measurement section
// // for details on how to interpret the content of this register
// uint8_t ADE7754_Temperature() { return ADE7754_read_8bit(ADE7754_REG_TEMP); }
// 
// // R(24)=0 Waveform Register. This register contains the digitized waveform of one
// // of the six analog inputs. The source is selected by Data Bits 0 to 2 in the
// // WAVMode register. See Table XIII.
// uint32_t ADE7754_Waveform() { return ADE7754_read_24bit(ADE7754_REG_WFORM); }
// 
// // RW(8)=4 Operational Mode Register. This register defines the general configuration
// // of the ADE7754. See Table IX.
// uint8_t ADE7754_OperationalMode() { return ADE7754_read_8bit(ADE7754_REG_OPMODE); }
// void ADE7754_OperationalMode(uint8_t val) { ADE7754_write_8bit(ADE7754_REG_OPMODE, val); }
// 
// // RW(8)=0x70 Measurement Mode Register. This register defines the channel used for
// // period and peak detection measurements. See Table XII.
// uint8_t ADE7754_MeasurementMode() { return ADE7754_read_8bit(ADE7754_REG_MMODE); }
// void ADE7754_MeasurementMode(uint8_t val) { ADE7754_write_8bit(ADE7754_REG_MMODE, val); }
// 
// // RW(8)=0 Waveform mode register. This register defines the channel and sampling
// // frequency used in waveform sampling mode. See Table XIII.
// uint8_t ADE7754_WaveformMode() { return ADE7754_read_8bit(ADE7754_REG_WAVMODE); }
// void ADE7754_WaveformMode(uint8_t val) { ADE7754_write_8bit(ADE7754_REG_WAVMODE, val); }
// 
// // RW(8)=0x3F This register configures the formula applied for the active energy and
// // line active energy measurements. See Table XIV.
// uint8_t ADE7754_WatMode() { return ADE7754_read_8bit(ADE7754_REG_WATMODE); }
// void ADE7754_WatMode(uint8_t val) { ADE7754_write_8bit(ADE7754_REG_WATMODE, val); }
// 
// // RW(8)=0x3F This register configures the formula applied for the apparent energy and
// // line apparent energy measurements. See Table XV.
// uint8_t ADE7754_VaMode() { return ADE7754_read_8bit(ADE7754_REG_VAMODE); }
// void ADE7754_VaMode(uint8_t val) { ADE7754_write_8bit(ADE7754_REG_VAMODE, val); }
// 
// // RW(16)=0 IRQ Enable Register. It determines whether an interrupt event will
// // generate an active low output at IRQ pin. See Table XVI
// uint16_t ADE7754_IRQEnable() { return ADE7754_read_16bit(ADE7754_REG_IRQEN); }
// void ADE7754_IRQEnable(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_IRQEN, val); }
// 
// // R(16)=0 IRQ Status Register. This register contains information regarding the
// // source of ADE7754 interrupts. See Table XVII.
// uint16_t ADE7754_IRQStatus() { return ADE7754_read_16bit(ADE7754_REG_STATUS); }
// 
// // R(16)=0 Same as the status register, except that its contents are reset to 0 (all
// // flags cleared) after a read operation.
// uint16_t ADE7754_IRQStatus_Reset() { return ADE7754_read_16bit(ADE7754_REG_RSTATUS); }
// 
// // RW(16)=0xFFFF Zero Cross Timeout Register. If no zero crossing is detected within a
// // time period specified by this register, the interrupt request line (IRQ)
// // will go active low for the corresponding line voltage. The maximum
// // timeout period is 2.3 seconds. See the Zero-Crossing Detection section.
// uint16_t ADE7754_ZeroCrossTimeout() { return ADE7754_read_16bit(ADE7754_REG_ZXTOUT); }
// void ADE7754_ZeroCrossTimeout(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_ZXTOUT, val); }
// 
// // RW(16)=0xFFFF Line Cycle Register. The content of this register sets the number of half line
// // cycles while the active energy and the apparent energy are accumulated in the
// // LAENERGY and LVAENERGY registers. See the Energy Calculation section
// uint16_t ADE7754_LineCycle() { return ADE7754_read_16bit(ADE7754_REG_LINCYC); }
// void ADE7754_LineCycle(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_LINCYC, val); }
// 
// // RW(8)=0xFF SAG Line Cycle Register. This register specifies the number of consecutive half
// // line cycles where voltage channel input falls below a threshold level.
// // This register is common to the three-line voltage SAG detection. The detection threshold
// // is specified by SAGLVL register. See the Line Voltage SAG Detection section
// uint8_t ADE7754_SAGLineCycle() { return ADE7754_read_8bit(ADE7754_REG_SAGCYC); }
// void ADE7754_SAGLineCycle(uint8_t val) { ADE7754_write_8bit(ADE7754_REG_SAGCYC, val); }
// 
// // RW(8)=0 SAG Voltage Level. This register specifies the detection threshold for
// // SAG event. This register is common to the three-line voltage SAG
// uint8_t ADE7754_SAGVoltageLevel() { return ADE7754_read_8bit(ADE7754_REG_SAGLVL); }
// void ADE7754_SAGVoltageLevel(uint8_t val) { ADE7754_write_8bit(ADE7754_REG_SAGLVL, val); }
// 
// // RW(8)=0xFF Voltage Peak Level. This register sets the level of the voltage peak
// // detection. If the selected voltage phase exceeds this level, the PKV flag
// // in the status register is set. See Table XII.
// uint8_t ADE7754_VoltagePeakLevel() { return ADE7754_read_8bit(ADE7754_REG_VPEAK); }
// void ADE7754_VoltagePeakLevel(uint8_t val) { ADE7754_write_8bit(ADE7754_REG_VPEAK, val); }
// 
// // RW(8)=0xFF Current Peak Level. This register sets the level of the current peak detection.
// // If the selected current phase exceeds this level, the PKI flag in the status register is set. See Table XII.
// uint8_t ADE7754_CurrentPeakLevel() { return ADE7754_read_8bit(ADE7754_REG_IPEAK); }
// void ADE7754_CurrentPeakLevel(uint8_t val) { ADE7754_write_8bit(ADE7754_REG_IPEAK, val); }
// 
// // RW(8)=0 PGA Gain Register. This register is used to adjust the gain selection for
// // the PGA in current and voltage channels. See the Analog Inputs section
// // and Table X. This register is also used to configure the active energy
// // accumulation no-load threshold and sum of absolute values.
// uint8_t ADE7754_PGAGain() { return ADE7754_read_8bit(ADE7754_REG_GAIN); }
// void ADE7754_PGAGain(uint8_t val) { ADE7754_write_8bit(ADE7754_REG_GAIN, val); }
// 
// // RW(12)=0 Phase A Active Power Gain Register. The active power caluation for Phase A
// // can be calibrated by writing to this register. The calibration range is 50% of the
// // nominal full-scale active power. The resolution of the gain adjust is 0.0244%/LSB.
// uint16_t ADE7754_ActivePowerGainA() { return ADE7754_read_16bit(ADE7754_REG_AWG); }
// void ADE7754_ActivePowerGainA(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_AWG, val); }
// 
// // RW(12)=0 Phase B Active Power Gain.
// uint16_t ADE7754_ActivePowerGainB() { return ADE7754_read_16bit(ADE7754_REG_BWG); }
// void ADE7754_ActivePowerGainB(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_BWG, val); }
// 
// // RW(12)=0 Phase C Active Power Gain
// uint16_t ADE7754_ActivePowerGainC() { return ADE7754_read_16bit(ADE7754_REG_CWG); }
// void ADE7754_ActivePowerGainC(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_CWG, val); }
// 
// // RW(12)=0 VA Gain Register. This register calculation can be calibrated by writing
// // this register. The calibration range is 50% of the nominal full-scale real
// // power. The resolution of the gain adjust is 0.02444%/LSB.
// uint16_t ADE7754_VAPowerGainA() { return ADE7754_read_16bit(ADE7754_REG_AVAG); }
// void ADE7754_VAPowerGainA(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_AVAG, val); }
// 
// // RW(12)=0 Phase B VA Gain.
// uint16_t ADE7754_VAPowerGainB() { return ADE7754_read_16bit(ADE7754_REG_BVAG); }
// void ADE7754_VAPowerGainB(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_BVAG, val); }
// 
// // RW(12)=0 Phase C VA Gain.
// uint16_t ADE7754_VAPowerGainC() { return ADE7754_read_16bit(ADE7754_REG_CVAG); }
// void ADE7754_VAPowerGainC(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_CVAG, val); }
// 
// 
// // RW(5)=0 Phase A Phase Calibration Register
// uint8_t ADE7754_PhaseCalibrationA() { return ADE7754_read_8bit(ADE7754_REG_APHCAL); }
// void ADE7754_PhaseCalibrationA(uint8_t val) { ADE7754_write_8bit(ADE7754_REG_APHCAL, val); }
// // RW(5)=0 Phase B Phase Calibration Register
// uint8_t ADE7754_PhaseCalibrationB() { return ADE7754_read_8bit(ADE7754_REG_BPHCAL); }
// void ADE7754_PhaseCalibrationB(uint8_t val) { ADE7754_write_8bit(ADE7754_REG_BPHCAL, val); }
// // RW(5)=0 Phase C Phase Calibration Register
// uint8_t ADE7754_PhaseCalibrationC() { return ADE7754_read_8bit(ADE7754_REG_CPHCAL); }
// void ADE7754_PhaseCalibrationC(uint8_t val) { ADE7754_write_8bit(ADE7754_REG_CPHCAL, val); }
// 
// 
// // RW(12)=0 Phase A Power Offset Calibration Register.
// uint16_t ADE7754_PowerOffsetCalibrationA() { return ADE7754_read_16bit(ADE7754_REG_AAPOS); }
// void ADE7754_PowerOffsetCalibrationA(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_AAPOS, val); }
// // RW(12)=0 Phase B Power Offset Calibration Register.
// uint16_t ADE7754_PowerOffsetCalibrationB() { return ADE7754_read_16bit(ADE7754_REG_BAPOS); }
// void ADE7754_PowerOffsetCalibrationB(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_BAPOS, val); }
// // RW(12)=0 Phase C Power Offset Calibration Register.
// uint16_t ADE7754_PowerOffsetCalibrationC() { return ADE7754_read_16bit(ADE7754_REG_CAPOS); }
// void ADE7754_PowerOffsetCalibrationC(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_CAPOS, val); }
// 
// // RW(12)=0 CF Scaling Numerator Register. The content of this register is used in
// // the numerator of CF output scaling.
// uint16_t ADE7754_CF_ScalingNumerator() { return ADE7754_read_16bit(ADE7754_REG_CFNUM); }
// void ADE7754_CF_ScalingNumerator(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_CFNUM, val); }
// 
// // RW(12)=3F CF Scaling Denominator Register. The content of this register is used in
// // the denominator of CF output scaling
// uint16_t ADE7754_CF_ScalingDenominator() { return ADE7754_read_16bit(ADE7754_REG_CFDEN); }
// void ADE7754_CF_ScalingDenominator(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_CFDEN, val); }
// 
// // RW(8)=0 Active Energy Register Divider.
// uint8_t ADE7754_ActiveEnergyDiv() { return ADE7754_read_8bit(ADE7754_REG_WDIV); }
// void ADE7754_ActiveEnergyDiv(uint8_t val) { ADE7754_write_8bit(ADE7754_REG_WDIV, val); }
// 
// // RW(8)=0 Apparent Energy Register Divider.
// uint8_t ADE7754_ApparentEnergyDiv() { return ADE7754_read_8bit(ADE7754_REG_VADIV); }
// void ADE7754_ApparentEnergyDiv(uint8_t val) { ADE7754_write_8bit(ADE7754_REG_VADIV, val); }
// 
// // R(24)=0 Phase A Current Channel RMS Register. The register contains the rms
// // component of one input of the current channel. The source is selected by data
// // bits in the mode register.
// uint32_t ADE7754_AIrms() { return ADE7754_read_24bit(ADE7754_REG_AIRMS); }
// // R(24)=0 Phase B Current Channel RMS Register.
// uint32_t ADE7754_BIrms() { return ADE7754_read_24bit(ADE7754_REG_BIRMS); }
// // R(24)=0 Phase C Current Channel RMS Register.
// uint32_t ADE7754_CIrms() { return ADE7754_read_24bit(ADE7754_REG_CIRMS); }
// // R(24)=0 Phase A Voltage Channel RMS Register
// uint32_t ADE7754_AVrms() { return ADE7754_read_24bit(ADE7754_REG_AVRMS); }
// // R(24)=0 Phase B Voltage Channel RMS Register
// uint32_t ADE7754_BVrms() { return ADE7754_read_24bit(ADE7754_REG_BVRMS); }
// // R(24)=0 Phase C Voltage Channel RMS Register
// uint32_t ADE7754_CVrms() { return ADE7754_read_24bit(ADE7754_REG_CVRMS); }
// 
// // RW(12)=0 Phase A Current RMS Offset Correction Register
// uint16_t ADE7754_AIrmsOS() { return ADE7754_read_16bit(ADE7754_REG_AIRMSOS); }
// void ADE7754_AIrmsOS(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_AIRMSOS, val); }
// 
// // RW(12)=0 Phase B Current RMS Offset Correction Register
// uint16_t ADE7754_BIrmsOS() { return ADE7754_read_16bit(ADE7754_REG_BIRMSOS); }
// void ADE7754_BIrmsOS(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_BIRMSOS, val); }
// 
// // RW(12)=0 Phase C Current RMS Offset Correction Register
// uint16_t ADE7754_CIrmsOS() { return ADE7754_read_16bit(ADE7754_REG_CIRMSOS); }
// void ADE7754_CIrmsOS(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_CIRMSOS, val); }
// 
// // RW(12)=0 Phase A Voltage RMS Offset Correction Register.
// uint16_t ADE7754_AVrmsOS() { return ADE7754_read_16bit(ADE7754_REG_AVRMSOS); }
// void ADE7754_AVrmsOS(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_AVRMSOS, val); }
// 
// // RW(12)=0 Phase B Voltage RMS Offset Correction Register.
// uint16_t ADE7754_BVrmsOS() { return ADE7754_read_16bit(ADE7754_REG_BVRMSOS); }
// void ADE7754_BVrmsOS(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_BVRMSOS, val); }
// 
// // RW(12)=0 Phase C Voltage RMS Offset Correction Register.
// uint16_t ADE7754_CVrmsOS() { return ADE7754_read_16bit(ADE7754_REG_CVRMSOS); }
// void ADE7754_CVrmsOS(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_CVRMSOS, val); }
// 
// // RW(12)=0 Phase A Active Power Gain Adjust. The active power accumulation of the
// // Phase A can be calibrated by writing to this register. The calibration range
// // is ±50% of the nominal full scale of the active power. The resolution of the
// // gain is 0.0244%/LSB. See the Current Channel ADC Gain Adjust section.
// uint16_t ADE7754_ActivePowerGainAdjustA() { return ADE7754_read_16bit(ADE7754_REG_AAPGAIN); }
// void ADE7754_ActivePowerGainAdjustA(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_AAPGAIN, val); }
// 
// // RW(12)=0 Phase B Active Power Gain Adjust
// uint16_t ADE7754_ActivePowerGainAdjustB() { return ADE7754_read_16bit(ADE7754_REG_BAPGAIN); }
// void ADE7754_ActivePowerGainAdjustB(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_BAPGAIN, val); }
// 
// // RW(12)=0 Phase C Active Power Gain Adjust
// uint16_t ADE7754_ActivePowerGainAdjustC() { return ADE7754_read_16bit(ADE7754_REG_CAPGAIN); }
// void ADE7754_ActivePowerGainAdjustC(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_CAPGAIN, val); }
// 
// // RW(12)=0 Phase A Voltage RMS Gain. The apparent power accumulation of the
// // Phase A can be calibrated by writing to this register. The calibration range
// // is ±50% of the nominal full scale of the apparent power. The resolution of
// // the gain is 0.0244% / LSB. See the Voltage RMS Gain Adjust section.
// uint16_t ADE7754_RMSVoltageGainA() { return ADE7754_read_16bit(ADE7754_REG_AVGAIN); }
// void ADE7754_RMSVoltageGainA(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_AVGAIN, val); }
// 
// // RW(12)=0 Phase B Voltage RMS Gain.
// uint16_t ADE7754_RMSVoltageGainB() { return ADE7754_read_16bit(ADE7754_REG_BVGAIN); }
// void ADE7754_RMSVoltageGainB(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_BVGAIN, val); }
// 
// // RW(12)=0 Phase C Voltage RMS Gain.
// uint16_t ADE7754_RMSVoltageGainC() { return ADE7754_read_16bit(ADE7754_REG_CVGAIN); }
// void ADE7754_RMSVoltageGainC(uint16_t val) { ADE7754_write_16bit(ADE7754_REG_CVGAIN, val); }
// 
// // R(8)? Check Sum Register. The content of this register represents the sum of
// // all 1s of the latest register read from the SPI port.
// uint8_t ADE7754_CheckSum() { return ADE7754_read_8bit(ADE7754_REG_CHKSUM); }
// 
// // R(8)=1 Version of the Die.
// uint8_t ADE7754_Version() { return ADE7754_read_8bit(ADE7754_REG_VERSION); }
// 
// 
// 
// 
// uint8_t ADE7754_TemperatureOffset() { return 129; }
// uint8_t ADE7754_TemperatureScale() { return 4; }
// 
// 
// void ADE7754_IRQ_Waveform(bool enable) {
//     uint16_t irqen = ADE7754_spi_read_reg_16(ADE7754_REG_IRQEN);
//     // Enables an interrupt when a data is present in the waveform register
//     if (enable) irqen |= 1 << 14;
//     else irqen &= ~(1 << 14);
//     ADE7754_spi_write_reg_16(ADE7754_REG_IRQEN, irqen);
// }
// 
// 
// 
// 
// void ADE7754_OperationalMode_ResetChip() {
//     uint8_t val = ADE7754_spi_read_reg_8(ADE7754_REG_OPMODE);
//     val |= 1 << 6; // Software Chip Reset
//     ADE7754_spi_write_reg_8(ADE7754_REG_OPMODE, val);
// }
// 
// 
// // Power down the device
// void ADE7754_OperationalMode_PoweredDown() {
//     uint8_t val = ADE7754_spi_read_reg_8(ADE7754_REG_OPMODE);
//     // ADE7754 powered down
//     val |= 7 << 3;
//     ADE7754_spi_write_reg_8(ADE7754_REG_OPMODE, val);
// }
// 
// void ADE7754_InitialSetup() {
//     // use low spi speed for init
//     // spi_mode = SPI_MODE_3;
//     // spi_setup(us);
//     // Disable IRQ
//     ADE7754_IRQEnable(0);
//     ADE7754_OperationalMode_ResetChip();
//     _delay_ms(ADE7754_STARTUP_DELAY);
// }
// 
// 
// // WaveformMode
// uint16_t ADE7754_Read_Sampling_Frequency() {
//     uint8_t t = ADE7754_spi_read_reg_8(ADE7754_REG_WAVMODE);
//     t = (t >> 3) & 0x3;
//     return (26000 / (1 + t));
// }
// 
// void ADE7754_Write_Sampling_Frequency(uint16_t val) {
//     if (val == 0) return;
//     uint8_t t = (26000 / val);
//     if (t > 0) t--;
//     // if (t > 1) max_speed_hz = ADE7754_SPI_SLOW;
//     // else max_speed_hz = ADE7754_SPI_FAST;
//     uint8_t reg = ADE7754_spi_read_reg_8(ADE7754_REG_WAVMODE);
//     reg &= ~(3 << 3);
//     reg |= t << 3;
//     ADE7754_spi_write_reg_8(ADE7754_REG_WAVMODE, reg);
// }
// 
