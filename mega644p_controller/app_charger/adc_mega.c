#include "mcu/mcu.h"
#include "adc_mega.h"

/*  ADC_1V1:  // 1v1       ADM*UX |= ((1 << REFS1) | (1 << REFS0));
    ADC_AREF: // ADC_AREF (default)    ADMUX &= ~((1 << REFS1) | (1 << REFS0));
    ADC_VCC:  // ADC_VCC    ADMUX &= ~(1 << REFS1); ADMUX |= (1 << REFS0);
*/

// R        0
// G        1
// B        2
// SET_I    3
// T1       4
// T2       5
// PP       6
// CP       7

#define ADC_CH_START    3
#define ADC_CH_END      7


#define ADC_REFERENCE REFS0


#define ADC_SET_I   3
#define ADC_T1      4
#define ADC_T2      5
#define ADC_PP      6
#define ADC_CP      7

#define ADC_MAX     6
#define ADC_START   3

volatile uint8_t  ch_id             = ADC_CP;
volatile uint8_t  ch_id_next        = ADC_START;
volatile uint16_t _ad_val           = 0;
volatile uint16_t _ad_instant[5]    = {0};
adc_avg_buf_16 _PP = {0}, _T2 = {0}, _T1 = {0}, _SETI = {0};
volatile uint16_t _ad_CP_max = 0;
volatile uint16_t _ad_CP_min = 1024;


ISR(ADC_vect) {
    ch_id               = ADMUX & 0x07;
    _ad_val             = ADCW;
    _ad_instant[ch_id - 3]  = _ad_val;

    switch (ch_id) {

        case ADC_CP:
            if (_ad_CP_max < _ad_val) { _ad_CP_max = _ad_val; }
            if (_ad_CP_min > _ad_val) { _ad_CP_min = _ad_val; }
            ch_id = ch_id_next;
            ++ch_id_next;
            if (ch_id_next > ADC_MAX) ch_id_next = ADC_START;
            break;

        case ADC_PP:
            adc_avg_push_16_16(&_PP, _ad_val);
            ch_id = ADC_CP;
            break;

        case ADC_T2:
            adc_avg_push_16_16(&_T2, _ad_val);
            ch_id = ADC_CP;
            break;

        case ADC_T1:
            adc_avg_push_16_16(&_T1, _ad_val);
            ch_id = ADC_CP;
            break;

        case ADC_SET_I:
            adc_avg_push_16_16(&_SETI, _ad_val);
            ch_id = ADC_CP;
            break;
    }

    ADMUX = (1 << ADC_REFERENCE) | ch_id;
    ADCSRA |= (1 << ADSC); // Start new conversion

}

uint16_t adc_avg_get_16_16(adc_avg_buf_16 *p) {
    return (p->sum/ADC_AVG_BUF);
}

void adc_avg_push_16_16(adc_avg_buf_16 *p, uint16_t val) {
    p->sum -= p->buffer[p->index];
    p->sum += val;
    p->buffer[p->index] = val;
    if (++p->index == ADC_AVG_BUF) { p->index = 0; };
}

// ADCSRA = ADEN ADSC ADATE ADIF ADIE ADPS2 ADPS1 ADPS0
// ADPS2  ADPS1   ADPS0 Division Factor
//  0       0       0       2
//  0       0       1       2
//  0       1       0       4
//  0       1       1       8
//  1       0       0       16
//  1       0       1       32
//  1       1       0       64
//  1       1       1       128
void AdcMega_init() {
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1); // speed (clk/64), interrupt enabled
    // ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // slow speed (clk/128), interrupt enabled
    ADMUX  = (1 << ADC_REFERENCE) | ch_id;
    ADCSRA |= (1 << ADSC); // Start conversion
}

volatile uint8_t _old_SREG;
volatile uint16_t _ret_16;

uint16_t AdcMega_get_val(const AdcVal channel) {
    _old_SREG = SREG;
    cli();

    switch (channel) {

        case instant_SET_I:
            _ret_16 = _ad_instant[(ADC_SET_I - 3)];
            break;

        case instant_ADC_T1:
            _ret_16 = _ad_instant[(ADC_T1 - 3)];
            break;

        case instant_ADC_T2:
            _ret_16 = _ad_instant[(ADC_T2 - 3)];
            break;

        case instant_ADC_PP:
            _ret_16 = _ad_instant[(ADC_PP - 3)];
            break;

        case instant_ADC_CP:
            _ret_16 = _ad_instant[(ADC_CP - 3)];
            break;

        case max_ADC_CP:
            _ret_16 = _ad_CP_max;
            _ad_CP_max = 0;
            break;

        case min_ADC_CP:
            _ret_16 = _ad_CP_min;
            _ad_CP_min = 1024;
            break;

        case avg_SET_I:
            _ret_16 = adc_avg_get_16_16(&_SETI);
            break;

        case avg_ADC_T1:
            _ret_16 = adc_avg_get_16_16(&_T1);
            break;

        case avg_ADC_T2:
            _ret_16 = adc_avg_get_16_16(&_T2);
            break;

        case avg_ADC_PP:
            _ret_16 = adc_avg_get_16_16(&_PP);
            break;
    }
    SREG = _old_SREG;
    return _ret_16;
}
