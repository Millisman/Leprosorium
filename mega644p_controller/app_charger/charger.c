#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include "charger.h"
#include "mcu/mcu.h"

#include "adc_mega.h"

void app_Charger_init() {
    AdcMega_init();
    printf_P(PSTR("app_Charger_init\n"));
}

void app_Charger_update() {}

void app_Charger_update_1s() {
    return;
    printf_P(PSTR("AVG SET_I=%u, ADC_T1=%u, ADC_T2=%u, ADC_PP=%u, ADC_CP_MIN=%u, ADC_CP_MAX=%u\n"),
           AdcMega_get_val(avg_SET_I),
           AdcMega_get_val(avg_ADC_T1),
           AdcMega_get_val(avg_ADC_T2),
           AdcMega_get_val(avg_ADC_PP),
           AdcMega_get_val(min_ADC_CP),
           AdcMega_get_val(max_ADC_CP)
           );
}

void app_Charger_update_100ms() {
    return;
    printf_P(PSTR("Instant SET_I=%u, ADC_T1=%u, ADC_T2=%u, ADC_PP=%u, ADC_CP=%u\n"),
           AdcMega_get_val(instant_SET_I),
           AdcMega_get_val(instant_ADC_T1),
           AdcMega_get_val(instant_ADC_T2),
           AdcMega_get_val(instant_ADC_PP),
           AdcMega_get_val(instant_ADC_CP)
           );
}
