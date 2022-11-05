#include "ADE7758.h"
#include <util/delay_basic.h>
#include <util/delay.h>
#include "mcu/pins.h"
#include "mcu/pin_macros.h"

void ADE7758_OperationalMode_ResetChip() {
    uint8_t val = ADE7758_spi_read_reg_8(ADE7758_REG_OPMODE);
    val |= 1 << 6; // Software Chip Reset
    ADE7758_spi_write_reg_8(ADE7758_REG_OPMODE, val);
}

void ADE7758_InitialSetup() {
    // use low spi speed for init
    // spi_mode = SPI_MODE_3;
    // spi_setup(us);
    // Disable IRQ
//     ADE7754_IRQEnable(0);
    ADE7758_OperationalMode_ResetChip();
    _delay_ms(ADE7758_STARTUP_DELAY);
}

uint8_t ADE7758_getCHKSUM() { return ADE7758_read_8bit(ADE7758_REG_CHKSUM); }
int8_t  ADE7758_getTEMP() { return (int8_t)ADE7758_read_8bit(ADE7758_REG_TEMP); }

/**
 * In general:
 * @params:  void
 * @return: register content (measure) of the proper type depending on register width
 */
uint8_t ADE7758_getVersion() { return ADE7758_read_8bit(ADE7758_REG_Version); }

/** === setOpMode / getOpMode ===
 * OPERATIONAL MODE REGISTER (0x13)
 * The general configuration of the ADE7758 is defined by writing to the OPMODE register. 
 * Table 18 summarizes the functionality of each bit in the OPMODE register.
  
Bit Location  Bit Mnemonic   Default Value   Description
  0                DISHPF          0           The HPFs in all current channel inputs are disabled when this bit is set.
  1                DISLPF          0           The LPFs after the watt and VAR multipliers are disabled when this bit is set.
  2                DISCF           1           The frequency outputs APCF and VARCF are disabled when this bit is set.
  3 to 5           DISMOD          0           By setting these bits, ADE7758’s ADCs can be turned off.
                                               In normal operation, these bits should be left at Logic 0.
                                                DISMOD[2:0] Description
                                                0    0    0 Normal operation.
                                                1    0    0 Redirect the voltage inputs to the signal paths
                                                            for the current channels and the current inputs to
                                                            the signal paths for the voltage channels.
                                                0    0    1 Switch off only the current channel ADCs.
                                                1    0    1 Switch off current channel ADCs and redirect the current
                                                            input signals to the voltage channel signal paths.
                                                0    1    0 Switch off only the voltage channel ADCs.
                                                1    1    0 Switch off voltage channel ADCs and redirect the voltage input signals to the current channel signal paths.
                                                0    1    1 Put the ADE7758 in sleep mode.
                                                1    1    1 Put the ADE7758 in power-down mode (reduces AIDD to 1 mA typ).
 6                  SWRST           0           Software Chip Reset. A data transfer to the ADE7758 should nottake
                                                place for at least 18 μs after a software reset. WARNING!
 7                  RESERVED        0           This should be left at 0.
*/

void ADE7758_setOpMode(uint8_t m){ ADE7758_write_8bit(ADE7758_REG_OPMODE, m); }
uint8_t ADE7758_getOpMode(){ return ADE7758_read_8bit(ADE7758_REG_OPMODE); }

/** === setMMode / getMMode ===
 * MEASUREMENT MODE REGISTER (0x14)
 * The configuration of the PERIOD and peak measurements made by the ADE7758 is defined by writing to the MMODE register. 
 * Table 19 summarizes the functionality of each bit in the MMODE register.
 * 
 * Bit Location  Bit Mnemonic  Default Value    Description
 * 0 to 1        FREQSEL            0               These bits are used to select the source of the measurement of the voltage line frequency.
 *                                                  FREQSEL1  FREQSEL0   Source
 *                                                      0       0           Phase A
 *                                                      0       1           Phase B
 *                                                      1       0           Phase C
 *                                                      1       1           Reserved
 * 2 to 4        PEAKSEL             7              These bits select the phases used for the voltage and current peak registers.
 *                                                  Setting Bit 2 switches the IPEAK and VPEAK registers to hold the absolute values 
 *                                                  of the largest current and voltage waveform (over a fixed number of half-line cycles) 
 *                                                  from Phase A. The number of half-line cycles is determined by the content of the 
 *                                                  LINECYC register. At the end of the LINECYC number of half-line cycles, the content 
 *                                                  of the registers is replaced with the new peak values. Similarly, setting Bit 3 turns 
 *                                                  on the peak detection for Phase B, and Bit 4 for Phase C. Note that if more than one 
 *                                                  bit is set, the VPEAK and IPEAK registers can hold values from two different phases, that is,
 *                                                  the voltage and current peak are independently processed (see the Peak Current Detection section).
 * 5 to 7        PKIRQSEL            7              These bits select the phases used for the peak interrupt detection. 
 *                                                  Setting Bit 5 switches on the monitoring of the absolute current and voltage waveform to Phase A.
 *                                                  Similarly, setting Bit 6 turns on the waveform detection for Phase B, and Bit 7 for Phase C.
 *                                                  Note that more than one bit can be set for detection on multiple phases. 
 *                                                  If the absolute values of the voltage or current waveform samples in the selected phases exceeds 
 *                                                  the preset level specified in the VPINTLVL or IPINTLVL registers the corresponding bit(s) in the 
 *                                                  STATUS registers are set (see the Peak Current Detection section).
 * 
 */

void ADE7758_setMMode(uint8_t m){ ADE7758_write_8bit(ADE7758_REG_MMODE, m); }
uint8_t ADE7758_getMMode(){ return ADE7758_read_8bit(ADE7758_REG_MMODE); }

/** === setWavMode / getWavMode ===
 * WAVEFORM MODE REGISTER (0x15)
 * The waveform sampling mode of the ADE7758 is defined by writing to the WAVMODE register. 
 * Table 20 summarizes the functionality of each bit in the WAVMODE register.
 * 
 * Bit Location   Bit Mnemonic  Default Value  Description
 * 0 to 1           PHSEL           0           These bits are used to select the phase of the waveform sample.
 *                                              PHSEL[1:0]      Source
 *                                              0    0          Phase A
 *                                              0    1          Phase B
 *                                              1    0          Phase C
 *                                              1    1          Reserved
 * 2 to 4           WAVSEL          0           These bits are used to select the type of waveform.
 *                                              WAVSEL[2:0]   Source
 *                                              0    0    0     Current
 *                                              0    0    1     Voltage
 *                                              0    1    0     Active Power Multiplier Output
 *                                              0    1    1     Reactive Power Multiplier Output
 *                                              1    0    0     VA Multiplier Output
 *                                              -Others-        Reserved
 * 5 to 6           DTRT            0           These bits are used to select the data rate.
 *                                              DTRT[1:0] Update Rate
 *                                              0    0      26.04 kSPS (CLKIN/3/128)
 *                                              0    1      13.02 kSPS (CLKIN/3/256)
 *                                              1    0      6.51 kSPS (CLKIN/3/512)
 *                                              1    1      3.25 kSPS (CLKIN/3/1024)
 * 7                VACF            0           Setting this bit to Logic 1 switches the VARCF output pin to an output 
 *                                              frequency that is proportional to the total apparent power (VA). 
 *                                              In the default state, Logic 0, the VARCF pin outputs a frequency proportional 
 *                                              to the total reactive power (VAR).
 */

void ADE7758_setWavMode(uint8_t m) { ADE7758_write_8bit(ADE7758_REG_WAVMODE, m); }
uint8_t ADE7758_getWavMode() { return ADE7758_read_8bit(ADE7758_REG_WAVMODE); }

/** === setCompMode / getCompMode ===
 * 
 * COMPUTATIONAL MODE REGISTER (0x16)
 * The computational method of the ADE7758 is defined by writing to the COMPMODE register. 
 * 
 * Bit Location Bit Mnemonic  Default Value Description
 * 0 to 1           CONSEL          0       These bits are used to select the input to the energy accumulation registers. 
 *                                          CONSEL[1:0] = 11 is reserved. IA, IB, and IC are IA, IB, and IC phase shifted by –90°, respectively.
 *                                          Registers  CONSEL[1, 0] = 00   CONSEL[1, 0] = 01           CONSEL[1, 0] = 10
 *                                          AWATTHR     VA × IA             VA × (IA – IB)             VA × (IA–IB)
 *                                          BWATTHR     VB × IB             0                          0
 *                                          CWATTHR     VC × IC             VC × (IC – IB)             VC × IC
 *                                          AVARHR      VA × IA             VA × (IA – IB)             VA × (IA–IB)
 *                                          BVARHR      VB × IB             0                          0
 *                                          CVARHR      VC × IC             VC × (IC – IB)             VC × IC
 *                                          AVAHR       VARMS × IARMS       VARMS × IARMS              VARMS × ARMS
 *                                          BVAHR       VBRMS × IBRMS       (VARMS + VCRMS)/2 × IBRMS  VARMS × IBRMS
 *                                          CVAHR       VCRMS × ICRMS       VCRMS × ICRMS              VCRMS × ICRMS
 
 * 2 to 4           TERMSEL         7       These bits are used to select the phases to be included in the APCF and VARCF pulse outputs. 
 *                                          Setting Bit 2 selects Phase A (the inputs to AWATTHR and AVARHR registers) to be included. 
 *                                          Bit 3 and Bit 4 are for Phase B and Phase C, respectively. 
 *                                          Setting all three bits enables the sum of all three phases to be included in the frequency outputs 
 *                                          (see the Active Power Frequency Output and the Reactive Power Frequency Output sections).
 
 * 5                ABS             0       Setting this bit places the APCF output pin in absolute only mode. 
 *                                          Namely, the APCF output frequency is proportional to the sum of the absolute values of the watt-hour 
 *                                          accumulation registers (AWATTHR, BWATTHR, and CWATTHR). 
 *                                          Note that this bit only affects the APCF pin and has no effect on the content of the corresponding 
 *                                          registers.
 *                                                    
 * 6                SAVAR           0       Setting this bit places the VARCF output pin in the signed adjusted mode. 
 *                                          Namely, the VARCF output frequency is proportional to the sign-adjusted sum of the VAR-hour accumulation 
 *                                          registers (AVARHR, BVARHR, and CVARHR). 
 *                                          The sign of the VAR is determined from the sign of the watt calculation from the corresponding phase, 
 *                                          that is, the sign of the VAR is flipped if the sign of the watt is negative, and if the watt is positive, 
 *                                          there is no change to the sign of the VAR. 
 *                                          Note that this bit only affects the VARCF pin and has no effect on the content of the corresponding 
 *                                          registers.
 *                                                    
 * 7                NOLOAD          0       Setting this bit activates the no-load threshold in the ADE7758.
 */

void ADE7758_setCompMode(uint8_t m) { ADE7758_write_8bit(ADE7758_REG_COMPMODE, m); }
uint8_t ADE7758_getCompMode() { return ADE7758_read_8bit(ADE7758_REG_COMPMODE); }

/** === setLcycMode / getLcycMode ===
 * 
 * LINE CYCLE ACCUMULATION MODE REGISTER (0x17)
 * The functionalities involved the line-cycle accumulation mode in the ADE7758 are defined by writing to the LCYCMODE register. 
 * 
 * Bit Location Bit Mnemonic  Default Value  Description
 * 
 * 0                LWATT       0               Setting this bit places the watt-hour accumulation registers 
 *                                              (AWATTHR, BWATTHR, and CWATTHR registers) into line-cycle accumulation mode.
 * 1                LVAR        0               Setting this bit places the VAR-hour accumulation registers (AVARHR, BVARHR, and CVARHR registers) 
 *                                              into line-cycle accumulation mode.
 * 2                LVA         0               Setting this bit places the VA-hour accumulation registers (AVAHR, BVAHR, and CVAHR registers) 
 *                                              into line-cycle accumulation mode.
 * 3 to 5           ZXSEL       7               These bits select the phases used for counting the number of zero crossings in the line-cycle 
 *                                              accumulation mode. Bit 3, Bit 4, and Bit 5 select Phase A, Phase B, and Phase C, respectively. 
 *                                              More than one phase can be selected for the zero-crossing detection, 
 *                                              and the accumulation time is shortened accordingly.
 * 6                RSTREAD     1               Setting this bit enables the read-with-reset for all the WATTHR, VARHR, and VAHR registers for all three
 *                                              phases, that is, a read to those registers resets the registers to 0 after the content of the registers
 *                                              have been read. This bit should be set to Logic 0 when the LWATT, LVAR, or LVA bits are set to Logic 1.
 * 7                FREQSEL     0               Setting this bit causes the FREQ (0x10) register to display the period, instead of the frequency of the 
 *                                              line input.
 */

void ADE7758_setLcycMode(uint8_t m) { ADE7758_write_8bit(ADE7758_REG_LCYCMODE, m); }
uint8_t ADE7758_getLcycMode() { return ADE7758_read_8bit(ADE7758_REG_LCYCMODE); }

/** === gainSetup ===
 * GAIN REGISTER (0x23)
 * The PGA configuration of the ADE7758 is defined by writing to the GAIN register. 
 * Table 18 summarizes the functionality of each bit in the GAIN register.
 * 
 * Bit Location Bit Mnemonic Default Value Description
 * 0 to 1           PGA1        0           Current GAIN
 *                                          PGA1[1:0]            Description
 *                                          0    0                x1
 *                                          0    1                x2
 *                                          1    0                x4
 *                                          1    1                RESERVED    
 * 2                    ----                RESERVED            Reserved.
 * 3 to 4           SCALE       0           Current input full-scale select
 *                                          SCALE[1:0]            Description
 *                                          0    0                0.5v
 *                                          0    1                0.25v
 *                                          1    0                0.125v
 *                                          1    1                Reserved
 * 5 to 6           PGA2        0           Voltage GAIN
 *                                          PGA2[1:0]            Description
 *                                          0    0                x1
 *                                          0    1                x2
 *                                          1    0                x4
 *                                          1    1                RESERVED    
 * 7                INTEGRATOR  0           This bit enables the integrator on the current chanel when set. 
 */

void ADE7758_gainSetup(uint8_t integrator, uint8_t scale, uint8_t PGA2, uint8_t PGA1)
{
    uint8_t gain_data = (integrator<<7) | (PGA2<<5) | (scale<<3) | (PGA1);
    
    
    gain_data = 0;
    
    
    
    //write GAIN register, format is |3 bits PGA2 gain|2 bits full scale|3 bits PGA1 gain
    ADE7758_write_8bit(ADE7758_REG_Gain, gain_data);
}

void ADE7758_setupDivs(uint8_t Watt_div,uint8_t VAR_div,uint8_t VA_div)
{
    ADE7758_write_8bit(ADE7758_REG_WDIV,    Watt_div);
    ADE7758_write_8bit(ADE7758_REG_VARDIV,  VAR_div);
    ADE7758_write_8bit(ADE7758_REG_VADIV,   VA_div);
}

/** === getMaskInterrupts / setMaskInterrupts
 * MASK REGISTER (0x18)
 * When an interrupt event occurs in the ADE7758, the IRQ logic output goes active low if the mask bit for this event is Logic 1 in the MASK register. 
 * The IRQ logic output is reset to its default collector open state when the RSTATUS register is read. 
 * describes the function of each bit in the interrupt mask register. 
 * 
 * Bit Location Interrupt Flag  Default Value Description
 * 0                AEHF            0           Enables an interrupt when there is a change in Bit 14 of any one of 
 *                                              the three WATTHR registers, that is, the WATTHR register is half full.
 * 1                REHF            0           Enables an interrupt when there is a change in Bit 14 of any one of
 *                                              the three VARHR registers, that is, the VARHR register is half full.
 * 2                VAEHF           0           Enables an interrupt when there is a 0 to 1 transition in the MSB of
 *                                              any one of the three VAHR registers, that is, the VAHR register is half full.
 * 3                SAGA            0           Enables an interrupt when there is a SAG on the line voltage of the Phase A.
 * 4                SAGB            0           Enables an interrupt when there is a SAG on the line voltage of the Phase B.
 * 5                SAGC            0           Enables an interrupt when there is a SAG on the line voltage of the Phase C.
 * 6                ZXTOA           0           Enables an interrupt when there is a zero-crossing timeout detection on Phase A.
 * 7                ZXTOB           0           Enables an interrupt when there is a zero-crossing timeout detection on Phase B.
 * 8                ZXTOC           0           Enables an interrupt when there is a zero-crossing timeout detection on Phase C.
 * 9                ZXA             0           Enables an interrupt when there is a zero crossing in the voltage channel of Phase A 
 *                                              (see the Zero-Crossing Detection section).
 * 10               ZXB             0           Enables an interrupt when there is a zero crossing in the voltage channel of Phase B 
 *                                              (see the Zero-Crossing Detection section).
 * 11               ZXC             0           Enables an interrupt when there is a zero crossing in the voltage channel of Phase C 
 *                                              (see the Zero-Crossing Detection section).
 * 12               LENERGY         0           Enables an interrupt when the energy accumulations over LINECYC are finished.
 * 13               RESERVED        0           Reserved.
 * 14               PKV             0           Enables an interrupt when the voltage input selected in the MMODE register is above 
 *                                              the value in the VPINTLVL register.
 * 15               PKI             0           Enables an interrupt when the current input selected in the MMODE register is above the 
 *                                              value in the IPINTLVL register.
 * 16               WFSM            0           Enables an interrupt when data is present in the WAVEMODE register.
 * 17               REVPAP          0           Enables an interrupt when there is a sign change in the watt calculation among any one of 
 *                                              the phases specified by the TERMSEL bits in the COMPMODE register.
 * 18               REVPRP          0           Enables an interrupt when there is a sign change in the VAR calculation among any one of 
 *                                              the phases specified by the TERMSEL bits in the COMPMODE register.
 * 19               SEQERR          0           Enables an interrupt when the zero crossing from Phase A is followed
 *                                              not by the zero crossing of Phase C but with that of Phase B.
 */
uint32_t ADE7758_getMaskInterrupts(void) { return ADE7758_read_24bit(ADE7758_REG_Mask); }
void ADE7758_setMaskInterrupts(uint32_t m) { return ADE7758_write_24bit(ADE7758_REG_Mask, m); }

/**    getStatus / resetStatus
 * INTERRUPT STATUS REGISTER (0x19)/RESET INTERRUPT STATUS REGISTER (0x1A)
 * The interrupt status register is used to determine the source of an interrupt event. 
 * When an interrupt event occurs in the ADE7758, the corresponding flag in the interrupt status register is set.
 * The IRQ pin goes active low if the corresponding bit in the interrupt mask register is set. 
 * When the MCU services the interrupt, it must first carry out a read from the interrupt status register to determine the source of the interrupt.
 * All the interrupts in the interrupt status register stay at their logic high state after an event occurs.
 * The state of the interrupt bit in the interrupt status register is reset to its default value once the reset interrupt status register is read.
 * 
 * Bit Location Interrupt Flag Default Value  Event Description
 * 0                AEHF        0               Indicates that an interrupt was caused by a change in Bit 14 among any one of the three 
 *                                              WATTHR registers, that is, the WATTHR register is half full.
 * 1                REHF        0               Indicates that an interrupt was caused by a change in Bit 14 among any one of the three 
 *                                              VARHR registers, that is, the VARHR register is half full.
 * 2                VAEHF       0               Indicates that an interrupt was caused by a 0 to 1 transition in Bit 15 among any one of the three 
 *                                              VAHR registers, that is, the VAHR register is half full.
 * 3                SAGA        0               Indicates that an interrupt was caused by a SAG on the line voltage of the Phase A.
 * 4                SAGB        0               Indicates that an interrupt was caused by a SAG on the line voltage of the Phase B.
 * 5                SAGC        0               Indicates that an interrupt was caused by a SAG on the line voltage of the Phase C.
 * 6                ZXTOA       0               Indicates that an interrupt was caused by a missing zero crossing on the line voltage of the Phase A.
 * 7                ZXTOB       0               Indicates that an interrupt was caused by a missing zero crossing on the line voltage of the Phase B.
 * 8                ZXTOC       0               Indicates that an interrupt was caused by a missing zero crossing on the line voltage of the Phase C.
 * 9                ZXA         0               Indicates a detection of a rising edge zero crossing in the voltage channel of Phase A.
 * 10               ZXB         0               Indicates a detection of a rising edge zero crossing in the voltage channel of Phase B.
 * 11               ZXC         0               Indicates a detection of a rising edge zero crossing in the voltage channel of Phase C.
 * 12               LENERGY     0               In line energy accumulation, indicates the end of an integration over an integer number of 
 *                                              half- line cycles (LINECYC). See the Calibration section.
 * 13               RESET       1               Indicates that the 5 V power supply is below 4 V. Enables a software reset of the ADE7758 
 *                                              and sets the registers back to their default values. This bit in the STATUS or RSTATUS register 
 *                                              is logic high for only one clock cycle after a reset event.
 * 14               PKV         0               Indicates that an interrupt was caused when the selected voltage input is above the value in the 
 *                                              VPINTLVL register.
 * 15               PKI         0               Indicates that an interrupt was caused when the selected current input is above the value 
 *                                              in the IPINTLVL register.
 * 16               WFSM        0               Indicates that new data is present in the waveform register.
 * 17               REVPAP      0               Indicates that an interrupt was caused by a sign change in the watt calculation among any 
 *                                              one of the phases specified by the TERMSEL bits in the COMPMODE register.
 * 18               REVPRP      0               Indicates that an interrupt was caused by a sign change in the VAR calculation among any 
 *                                              one of the phases specified by the TERMSEL bits in the COMPMODE register.
 * 19               SEQERR      0               Indicates that an interrupt was caused by a zero crossing from Phase A 
 *                                              followed not by the zero crossing of Phase C but by that of Phase B.
 */

uint32_t ADE7758_getStatus(void) { return ADE7758_read_24bit(ADE7758_REG_Status); }
uint32_t ADE7758_resetStatus(void){ return ADE7758_read_24bit(ADE7758_REG_RSTATUS); }

/** === getFreq ===
 * Period of the Phase selected in MMode.
 * @param none
 * @return uint16_t with the data (12 bits unsigned).
 */
uint16_t ADE7758_getFreq(void){
    return ADE7758_read_16bit(ADE7758_REG_FREQ);
}
/** setLineCyc настроить количество последовательных циклов для накоплений.
 **/
void ADE7758_setLineCyc(uint16_t d){
    ADE7758_write_16bit(ADE7758_REG_LINECYC,d);
}
/** === setCurrentOffset / getCurrentOffset ===
 * @param none
 * @return uint16_t with the data (12 bits 2-complement signed).
 */
uint16_t ADE7758_getACurrentOffset(){
    return ((ADE7758_read_16bit(ADE7758_REG_AIRMSOS))<<4)>>4;
}
uint16_t ADE7758_getBCurrentOffset(){
    return ((ADE7758_read_16bit(ADE7758_REG_BIRMSOS))<<4)>>4;
}
uint16_t ADE7758_getCCurrentOffset(){
    return ((ADE7758_read_16bit(ADE7758_REG_CIRMSOS))<<4)>>4;
}
void ADE7758_setACurrentOffset(uint16_t o){
    ADE7758_write_16bit(ADE7758_REG_AIRMSOS, o);
}
void ADE7758_setBCurrentOffset(uint16_t o){
    ADE7758_write_16bit(ADE7758_REG_BIRMSOS, o);
}
void ADE7758_setCCurrentOffset(uint16_t o){
    ADE7758_write_16bit(ADE7758_REG_CIRMSOS, o);
}
/** === setVoltageOffset / getVoltageOffset ===
 * @param none
 * @return uint16_t with the data (12 bits 2-complement signed).
 */
uint16_t ADE7758_getAVoltageOffset(){
    return ((ADE7758_read_16bit(ADE7758_REG_AVRMSOS))<<4)>>4;
}
uint16_t ADE7758_getBVoltageOffset(){
    return ((ADE7758_read_16bit(ADE7758_REG_BVRMSOS))<<4)>>4;
}
uint16_t ADE7758_getCVoltageOffset(){
    return ((ADE7758_read_16bit(ADE7758_REG_CVRMSOS))<<4)>>4;
}
void ADE7758_setAVoltageOffset(uint16_t o){
    ADE7758_write_16bit(ADE7758_REG_AVRMSOS, o);
}
void ADE7758_setBVoltageOffset(uint16_t o){
    ADE7758_write_16bit(ADE7758_REG_BVRMSOS, o);
}
void ADE7758_setCVoltageOffset(uint16_t o){
    ADE7758_write_16bit(ADE7758_REG_CVRMSOS, o);
}
/** === setZeroCrossingTimeout / getZeroCrossingTimeout ===
 * Zero-Crossing Timeout. If no zero crossings are detected
 * on Channel 2 within a time period specified by this 12-bit register,
 * the interrupt request line (IRQ) is activated
 * @param none
 * @return uint16_t with the data (12 bits unsigned).
 */
void ADE7758_setZeroCrossingTimeout(uint16_t d){
    ADE7758_write_16bit(ADE7758_REG_ZXTOUT,d);
}
uint16_t ADE7758_getZeroCrossingTimeout(){
    return ADE7758_read_16bit(ADE7758_REG_ZXTOUT);
}

/** === setDefaults ===
 * Reset resister state
 * @param void
 * @return void
 *


void ADE7758_setDefaults(void)
{
    write_8bit(ADE7758_REG_OPMODE, 4);
    write_8bit(ADE7758_REG_MMODE, 0xFC);
    write_8bit(ADE7758_REG_WAVMODE, 0);
    write_8bit(ADE7758_REG_COMPMODE, 0x1C);
    write_8bit(ADE7758_REG_LCYCMODE, 0x78);
    write_24bit(ADE7758_REG_Mask, 0);
    write_16bit(ADE7758_REG_ZXTOUT, 0xFFFF);
    write_16bit(ADE7758_REG_LINECYC, 0xFFFF);
    write_8bit(ADE7758_REG_SAGCYC, 0xFF);
    write_8bit(ADE7758_REG_SAGLVL, 0);
    write_8bit(ADE7758_REG_VPINTLVL, 0xFF);
    write_16bit(ADE7758_REG_IPINTLVL, 0xFF);
    write_8bit(ADE7758_REG_Gain, 0);
    write_16bit(ADE7758_REG_AVRMSGAIN, 0);
    write_16bit(ADE7758_REG_BVRMSGAIN, 0);
    write_16bit(ADE7758_REG_CVRMSGAIN, 0);
    write_16bit(ADE7758_REG_AIGAIN, 0);
    write_16bit(ADE7758_REG_BIGAIN, 0);
    write_16bit(ADE7758_REG_CIGAIN, 0);
    write_16bit(ADE7758_REG_AWG, 0);
    write_16bit(ADE7758_REG_BWG, 0);
    write_16bit(ADE7758_REG_CWG, 0);
    write_16bit(ADE7758_REG_AVARG, 0);
    write_16bit(ADE7758_REG_BVARG, 0);
    write_16bit(ADE7758_REG_CVARG, 0);
    write_16bit(ADE7758_REG_AVAG, 0);
    write_16bit(ADE7758_REG_BVAG, 0);
    write_16bit(ADE7758_REG_CVAG, 0);
    write_16bit(ADE7758_REG_AVRMSOS, 0);
    write_16bit(ADE7758_REG_BVRMSOS, 0);
    write_16bit(ADE7758_REG_CVRMSOS, 0);
    write_16bit(ADE7758_REG_AIRMSOS, 0);
    write_16bit(ADE7758_REG_BIRMSOS, 0);
    write_16bit(ADE7758_REG_CIRMSOS, 0);
    write_16bit(ADE7758_REG_AWATTOS, 0);
    write_16bit(ADE7758_REG_BWATTOS, 0);
    write_16bit(ADE7758_REG_CWATTOS, 0);
    write_16bit(ADE7758_REG_AVAROS, 0);
    write_16bit(ADE7758_REG_BVAROS, 0);
    write_16bit(ADE7758_REG_CVAROS, 0);
    write_8bit(ADE7758_REG_APHCAL, 0);
    write_8bit(ADE7758_REG_BPHCAL, 0);
    write_8bit(ADE7758_REG_CPHCAL, 0);
    write_8bit(ADE7758_REG_WDIV, 0);
    write_8bit(ADE7758_REG_VARDIV, 0);
    write_8bit(ADE7758_REG_VADIV, 0);
    write_16bit(ADE7758_REG_APCFNUM, 0);
    write_16bit(ADE7758_REG_APCFDEN, 0x3F);
    write_16bit(ADE7758_REG_VARCFNUM, 0);
    write_16bit(ADE7758_REG_VARCFDEN, 0x3F);
}

*/

