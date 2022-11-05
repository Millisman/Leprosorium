#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include "gsm.h"
#include "mcu/mcu.h"
#include "mcu/uart.h"
#include "mcu/pin_macros.h"
#include "mcu/pins.h"
#include "gsm_proc.h"

extern ModemRxBuff_t __mdm_rx;

static SIM800_Man _Gsm_Modem;
static FILE __file_modem;

int __putc_modem(char ch, FILE *stream) {
    (void)stream;
      uart0_putc(ch);
    _hal_modem_write(ch);
    return 0;
}

void __gsm_on_off(bool on) {
    on ? ON(PIN_GSM_POWER) : OFF(PIN_GSM_POWER);
}

void app_Gsm_init() {
    printf("app_Gsm_init\n");
    fdev_setup_stream( &__file_modem, __putc_modem, NULL, _FDEV_SETUP_WRITE);
    stdin = &__file_modem;

    DRIVER(PIN_GSM_POWER, OUT);
    _hal_modem_init_port();
    _Gsm_Modem.Gsm_Power = __gsm_on_off;
    _Gsm_Modem.gsm_mode = 1; /// TODO
    if (_Gsm_Modem.gsm_mode) {
        printf_P(PSTR("enabled: %u\n"), _Gsm_Modem.gsm_mode);
        gsm_proc_begin(&_Gsm_Modem);
    }
}

void app_Gsm_update() {
    if (_Gsm_Modem.gsm_mode) {
        gsm_proc_update(&_Gsm_Modem, &__mdm_rx);
    }
}

void app_Gsm_update_100ms() {
    if (_Gsm_Modem.gsm_mode) {
        gsm_proc_update_100ms(&_Gsm_Modem, &__mdm_rx);
    }
}

void app_Gsm_update_1s() {
    if (_Gsm_Modem.gsm_mode) {
        gsm_proc_update_1s(&_Gsm_Modem, &__mdm_rx);
    }
}
