#pragma once

#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "ADE7758.h"
// #include "event/handler.h"
// #include "utils/cpp.h" 
// #include "events.h"
// #include "event/loop.h"
#include "utils/crc.h"
// #include "protocol/conn_types.h"
// #include "store/settings.h"

// namespace meter {

enum EnergyType { W, VAR, VA };
enum Phase      { SUMM, A, B, C };


typedef struct __attribute__((__packed__)) {
    int32_t     WATT_HR_SUM;    // Active Energy Register summ
    int32_t     VAR_HR_SUM;     // Reactive Energy Register summ
    int32_t     VA_HR_SUM;      // Apparent Energy Register summ
    uint32_t    ts;
    uint8_t     crc8;
} s_EMeter_Block;

typedef struct __attribute__((__packed__)) {
    int32_t     WATT_HR[3]; // Active Energy Register summ
    int32_t     VAR_HR[3];  // Reactive Energy Register summ
    int32_t     VA_HR[3];   // Apparent Energy Register summ
} s_EMeter_SessBlock;



typedef struct __attribute__((__packed__)) {
    uint32_t    VRMS[3];    // Phase A,B,C Voltage Channel RMS Register
    uint32_t    VRMS_MAX;
    uint32_t    IRMS[3];    // Phase A,B,C Current Channel RMS Register
    uint32_t    IRMS_MAX;
    uint16_t    FREQ;      // Frequency of the Line Input Register
    uint8_t     TEMP;      // Temperature Register
    uint32_t    WFORM;     // Waveform Register
} MetMeasure;


// class Meter:  public event::Handler {
//     stream::UartStream cout;
//     store::EepromSettings &settings;
//     devices::ADE7758 ade;
    s_EMeter_Block EmData;
    MetMeasure measure;
    uint32_t moment_state;
    uint8_t state_energy;
    uint8_t state_measure;
    uint32_t Reg_STATUS, Reg_STATUS_prev;
// // public:
//     Meter();  
//     meter::s_EMeter_SessBlock  SessEnergy; // ext used
    void print_StoredEnergy();
    void print_Energy();
    void print_Measure();
    void print_Status();
    void print_Details();

    
    int8_t get_TempC();
    
    uint32_t get_VRMS_max() { return measure.VRMS_MAX; }
    uint32_t get_IRMS_max() { return measure.IRMS_MAX; }
    
    
//     static Meter& init() {
//         static Meter meter_driver;
//         return meter_driver;
//     }

    uint8_t get_charging_amperage();

    void reset_SessEnergy();
    int32_t get_SessEnergy(EnergyType et, Phase ph);
    int32_t get_StoredEnergy(EnergyType et);
    void append_SessEnergy();
    
    enum StoreCmd {S_UNK, S_READ, S_DEFS, S_WRITE};
    void eeprom_store(StoreCmd o);
    
// private:    
//     void onEvent(const event::Event& event) override;
//     DISALLOW_COPY_AND_ASSIGN(Meter);
// };

// }
