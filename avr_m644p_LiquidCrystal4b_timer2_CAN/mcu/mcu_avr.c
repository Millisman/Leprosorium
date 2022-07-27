#include "mcu.h"

#if defined (__AVR__) || defined (__AVR_ARCH__)



#include <util/atomic.h>
#include <util/delay_basic.h>
#include "LiquidCrystal4b.h"

#include <stdio.h>
#include "uart.h"
#include "pin_macros.h"
#include "pins.h"



static volatile bool elapsed_100ms = false;
static volatile bool elapsed_1_sec = false;
static volatile uint8_t  __counter_10   = 0;
static volatile uint8_t  __counter_100  = 0;
static volatile uint8_t  __counter_100x10 = 0;
static volatile uint32_t timer0_millis = 0;
static volatile uint32_t timer0_uptime_s = 0;
//-------------------------------------------------
static volatile uint8_t _lcd_data_buffer[64] = {0};
static volatile uint8_t _lcd_data_comdata[8] = {0};
static volatile uint8_t _lcd_index_insert = 0;
static volatile uint8_t _lcd_index_read = 0;
static volatile mcu_Write_4b_modes __lcd4b_state = LCD_4b_IDLE;

void mcu_lcd_insert(const bool rs, const uint8_t ch) {
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        uint8_t a = _lcd_index_insert / 8;
        uint8_t b = _lcd_index_insert % 8;
        if (rs) {
            _lcd_data_comdata[a] |= (1U << b);
        } else {
            _lcd_data_comdata[a] &= (~(1U << b));
        }
        _lcd_data_buffer[_lcd_index_insert] = ch;
        ++_lcd_index_insert;
        if ( _lcd_index_insert >= sizeof(_lcd_data_buffer) ) { _lcd_index_insert = 0; }
        if ( _lcd_index_insert == _lcd_index_read ) { ++_lcd_index_read; }
    }
}



void mcu_lcd_write4b(uint8_t value) {
    _delay_loop_1(255);
    if ((value >> 0) & 0x01) { ON(PIN_LCD_D4); } else { OFF(PIN_LCD_D4); }
    if ((value >> 1) & 0x01) { ON(PIN_LCD_D5); } else { OFF(PIN_LCD_D5); }
    if ((value >> 2) & 0x01) { ON(PIN_LCD_D6); } else { OFF(PIN_LCD_D6); }
    if ((value >> 3) & 0x01) { ON(PIN_LCD_D7); } else { OFF(PIN_LCD_D7); }
    _delay_loop_1(255);
}


ISR(TIMER2_COMPA_vect) {
    ++__counter_10;

    if ( __counter_10 >= 10) {
        __counter_10 = 0;
        ++timer0_millis;    // millis
        ++__counter_100;

        if ( __counter_100 >= 100) {
            __counter_100 = 0;
            elapsed_100ms = true;
            ++__counter_100x10;

            if ( __counter_100x10 >= 10) {
                __counter_100x10 = 0;
                ++timer0_uptime_s;  // sec
                elapsed_1_sec = true;
            }

        }
    } else {

        switch (__lcd4b_state) {
            case LCD_4b_IDLE: {
                if ( _lcd_index_read != _lcd_index_insert ) {
                    uint8_t a = _lcd_index_read / 8;
                    uint8_t b = _lcd_index_read % 8;
                    if (_lcd_data_comdata[a] & (1UL << b)) {
                        ON(PIN_LCD_RS);
                    } else {
                        OFF(PIN_LCD_RS);
                    }
                    OFF(PIN_LCD_EN);
                    __lcd4b_state = LCD_4b_HIGH;
                }
            }
            break;

            case LCD_4b_HIGH: {
                OFF(PIN_LCD_EN);
                uint8_t a = _lcd_data_buffer[_lcd_index_read] >> 4;
                mcu_lcd_write4b(a);
                __lcd4b_state = LCD_4b_HIGH_W;
            }
            break;

            case LCD_4b_HIGH_W:
                ON(PIN_LCD_EN);
                __lcd4b_state = LCD_4b_LOW;
                break;

            case LCD_4b_LOW: {
                OFF(PIN_LCD_EN);
                uint8_t a = _lcd_data_buffer[_lcd_index_read];
                mcu_lcd_write4b(a);
                __lcd4b_state = LCD_4b_LOW_W;
            }
            break;

            case LCD_4b_LOW_W:
                ON(PIN_LCD_EN);
                __lcd4b_state = LCD_4b_DONE;
                break;

            case LCD_4b_DONE:
                OFF(PIN_LCD_EN);
                OFF(PIN_LCD_RS);
                ++_lcd_index_read;
                if ( _lcd_index_read >= sizeof(_lcd_data_buffer) ) _lcd_index_read = 0;
                __lcd4b_state = LCD_4b_IDLE;
                break;
        }
    }

}

uint32_t millis() {
    uint32_t r;
    ATOMIC_BLOCK(ATOMIC_FORCEON) { r = timer0_millis; }
    return r;
}

uint32_t uptime_sec() {
    uint32_t r;
    ATOMIC_BLOCK(ATOMIC_FORCEON) { r = timer0_uptime_s; }
    return r;
}

mcu_Timer_Elapses mcu_get_elapses() {
    if (elapsed_100ms) {
        elapsed_100ms = false;
        return ELAPSED_100ms;
    }
    if (elapsed_1_sec) {
        elapsed_1_sec = false;
        return ELAPSED_1S;
    }
    return NOT_ELAPSED;
}



void mcu_wdt_restart() {
    wdt_enable(WDTO_15MS);
    while (1) {}
}

int __putc_uart0(char ch, FILE *stream) {
    (void)stream;
    if (ch == '\n') uart0_putc('\r');
    uart0_putc(ch);
    return 0;
}

int __putc_lcd0(char ch, FILE *stream) {
    (void)stream;
    LiquidCrystal_send(ch, LCD_SEND_DATA);
    return 0;
}

static FILE __file_uart0;
static FILE __file_lcd0;

void mcu_init_all() {
    MCUSR = 0;
    wdt_enable(WDTO_2S);
    wdt_reset();

    //-------------------------------------------------- PINS
    DRIVER(PIN_LED_RED,     OUT);   ON(PIN_LED_RED);
    DRIVER(PIN_LED_GREEN,   OUT);   OFF(PIN_LED_GREEN);
    DRIVER(PIN_LED_BLUE,    OUT);   OFF(PIN_LED_BLUE);
    DRIVER(PIN_CS_ADE,      OUT);   ON(PIN_CS_ADE);
    DRIVER(PIN_CS_NFC,      OUT);   ON(PIN_CS_NFC);
    DRIVER(PIN_GFI,         IN);
    DRIVER(PIN_RF24_CE,     OUT);   OFF(PIN_RF24_CE);
    DRIVER(PIN_RF24_CSN,    OUT);   ON(PIN_RF24_CSN);

    DRIVER(PIN_MOSI,    OUT);   ON(PIN_MOSI);   //  PB5 Active High MOSI
    DRIVER(PIN_MISO,    IN);                    //  PB6 Active High MISO
    DRIVER(PIN_SCK,     OUT);   ON(PIN_SCK);    //  PB7 Active High SCK
    // ALERT skipping LCD pins
    DRIVER(PIN_MENNEKES_LOCK,   OUT);   OFF(PIN_MENNEKES_LOCK);
    DRIVER(PIN_MENNEKES_UNLOCK, OUT);   OFF(PIN_MENNEKES_UNLOCK);
    DRIVER(PIN_PILO,            OUT);   OFF(PIN_PILO);
    DRIVER(PIN_GSM_POWER,       OUT);   OFF(PIN_GSM_POWER);
    DRIVER(PIN_GFITEST,         OUT);   OFF(PIN_GFITEST);
    DRIVER(PIN_AC_RELAY,        OUT);   OFF(PIN_AC_RELAY);
    //--------------------------------------------------

    // http://www.8bit-era.cz/arduino-timer-interrupts-calculator.html
    // TIMER 2 for interrupt frequency 10000 Hz:
    TCCR2A = 0; // set entire TCCR2A register to 0
    TCCR2B = 0; // same for TCCR2B
    TCNT2  = 0; // initialize counter value to 0
    // set compare match register for 10000 Hz increments
    OCR2A = 249; // = 20000000 / (8 * 10000) - 1 (must be <256)
    // turn on CTC mode
    TCCR2B |= (1 << WGM21);
    // Set CS22, CS21 and CS20 bits for 8 prescaler
    TCCR2B |= (0 << CS22) | (1 << CS21) | (0 << CS20);
    // enable timer compare interrupt
    TIMSK2 |= (1 << OCIE2A);
    // TIMER2_COMPA_vect

    uart_init(UART_BAUD_SELECT(115200, F_CPU));
    fdev_setup_stream( &__file_uart0, __putc_uart0, NULL, _FDEV_SETUP_WRITE);
    stdout = &__file_uart0;
    fdev_setup_stream( &__file_lcd0, __putc_lcd0,   NULL, _FDEV_SETUP_WRITE);
    stderr = &__file_lcd0;
    sei();
    LiquidCrystal_begin(20, 4, LCD_5x8DOTS);
}


#endif
