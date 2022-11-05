#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <util/atomic.h>

// R = 0 G = 1 B = 2
// SET_I    3
// T1       4
// T2       5
// PP       6
// CP       7

#define ADC_AVG_BUF     16 // max64, mem*2

typedef struct __attribute__((__packed__)) {
    uint16_t buffer[ADC_AVG_BUF];
    uint16_t sum;
    uint8_t  index;
}
adc_avg_buf_16;

uint16_t adc_avg_get_16_16(adc_avg_buf_16 *p);
void adc_avg_push_16_16(adc_avg_buf_16 *p, uint16_t val);

typedef enum {
    instant_SET_I,
    instant_ADC_T1,
    instant_ADC_T2,
    instant_ADC_PP,
    instant_ADC_CP,
    max_ADC_CP,
    min_ADC_CP,
    avg_SET_I,
    avg_ADC_T1,
    avg_ADC_T2,
    avg_ADC_PP
} AdcVal;

void AdcMega_init();
uint16_t AdcMega_get_val(const AdcVal channel);
// uint16_t AdcMega_get_ADC(const uint8_t channel);
// uint8_t AdcMega_get_ZC(const uint8_t channel);
//
// bool     AdcMega_get_peak_trip(const uint8_t channel);
// uint16_t AdcMega_get_AD_max(const uint8_t channel);
// uint16_t AdcMega_get_AD_min(const uint8_t channel);
