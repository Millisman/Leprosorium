#include "mcu.h"

#if defined (__AVR__) || defined (__AVR_ARCH__)

#include "errno-base.h"
#include <util/twi.h>



#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define MICROSECONDS_PER_TIMER2_OVERFLOW    (clockCyclesToMicroseconds(64 * 256))
#define MILLIS2_INC     (MICROSECONDS_PER_TIMER2_OVERFLOW / 1000)
#define FRACT2_INC      ((MICROSECONDS_PER_TIMER2_OVERFLOW % 1000) >> 3)
#define FRACT2_MAX      (1000 >> 3)
#define MICROS2_MULT    (64 / clockCyclesPerMicrosecond())

extern bool     alert_interrupt_flag;
extern time_t   alert_interrupt_timestamp;

#define PIN_ALERT_BQ D,2,H

static volatile bool        _flag_250ms = false;
static volatile bool        _flag_1_sec = false;
static volatile uint8_t     counter_3       = 0;
static volatile uint8_t     counter_250     = 0;
static volatile uint8_t     counter_250x4   = 0;
static volatile uint32_t    timer2_millis   = 0;
static volatile uint32_t    timer2_uptime_s = 0;

ISR(TIMER2_COMPA_vect) {
    ++counter_3;
    if ( counter_3 >= 3) {
        counter_3 = 0;
        timer2_millis  += 2;
        counter_250 += 2;

        if ( counter_250 >= 250) {
            counter_250 = 0;
            _flag_250ms = 1;

            ++counter_250x4;
            if ( counter_250x4 >= 4) {
                counter_250x4 = 0;
                ++timer2_uptime_s;
                _flag_1_sec = 1;
            }

            if (ACTIVE(PIN_ALERT_BQ)) {
                alert_interrupt_timestamp = timer2_uptime_s;
                alert_interrupt_flag = true;
            }

        }

     } /*else {
         sleep_enable();
         sleep_cpu();
    }*/
}

uint32_t millis2() {
    uint32_t r;
    ATOMIC_BLOCK(ATOMIC_FORCEON) { r = timer2_millis; }
    return r;
}

uint32_t uptime2() {
    uint32_t r;
    ATOMIC_BLOCK(ATOMIC_FORCEON) { r = timer2_uptime_s; }
    return r;
}

bool elapsed_250ms() {
    if (_flag_250ms) {
        ATOMIC_BLOCK(ATOMIC_FORCEON) { _flag_250ms = false; }
        return true;
    }
    return false;
}


bool elapsed_one_sec() {
    if (_flag_1_sec) {
        ATOMIC_BLOCK(ATOMIC_FORCEON) { _flag_1_sec = false; }
        return true;
    }
    return false;
}

ISR(INT0_vect) {
    alert_interrupt_timestamp = timer2_uptime_s;
    alert_interrupt_flag = true;
}

extern uint32_t time_pressed;

ISR(INT1_vect) {
    time_pressed = millis2();
}

int uart0_putc2(char ch, FILE *stream) {
    (void)stream;
    if (ch == '\n') uart0_putc('\r');
    uart0_putc(ch);
    return 0;
}

#define CHANGE  1
#define FALLING 2
#define RISING  3

static FILE uart0_file;

void mcu_init() {
    MCUSR = 0;
    wdt_disable();
    power_adc_disable();
    power_spi_disable();
    power_timer0_disable();
    power_timer1_disable();

//     wdt_enable(WDTO_2S);
//     wdt_reset();



    // http://www.8bit-era.cz/arduino-timer-interrupts-calculator.html
    // -------- Timer2 init for millis2
    // TIMER 2 for interrupt frequency 750 Hz: // 3 tick = 4ms on 12mHz

    // TIMER 2 for interrupt frequency 1556.0165975103735 Hz:
    cli(); // stop interrupts
    TCCR2A = 0; // set entire TCCR2A register to 0
    TCCR2B = 0; // same for TCCR2B
    TCNT2  = 0; // initialize counter value to 0
    // set compare match register for 1556.0165975103735 Hz increments
    OCR2A = 240; // = 12000000 / (32 * 1556.0165975103735) - 1 (must be <256)
    // turn on CTC mode
    TCCR2B |= (1 << WGM21);
    // Set CS22, CS21 and CS20 bits for 32 prescaler
    TCCR2B |= (0 << CS22) | (1 << CS21) | (1 << CS20);
    // enable timer compare interrupt
    TIMSK2 |= (1 << OCIE2A);
    sei(); // allow interrupts


    // -------- Int0 init - ALERT   PD2
    EICRA = (1 << ISC01) | (1 << ISC00) | (RISING << ISC00);
    EIMSK = (1 << INT0);

    // -------- Int1 init - BUTTON  PD3
    EICRA = (1 << ISC11) | (1 << ISC10) | (FALLING << ISC10);
    EIMSK = (1 << INT1);

    uart0_init(UART_BAUD_SELECT_DOUBLE_SPEED(115200, 12000000L));
    //     millis_begin();
    //     fdev_setup_stream( &file_uart0, putc_uart0, NULL, _FDEV_SETUP_WRITE);
    //     stdout = &file_uart0; //  = stdin = stderr
    //     // fdev_setup_stream( &file_lcd0, putc_lcd0,   NULL, _FDEV_SETUP_WRITE);
    //     // stderr = &file_lcd0;
    //     sei();

    fdev_setup_stream( &uart0_file, uart0_putc2, NULL, _FDEV_SETUP_WRITE);
    stdout = &uart0_file;

    DRIVER(PIN_ALERT_BQ, IN); //as input
    sei();
    printf_P(PSTR("mcu_init\n"));

}

// https://ccrma.stanford.edu/courses/250a-fall-2005/docs/avr-libc-user-manual-1.2.5/group__twi__demo.html



#define I2C_RESET (1 << TWEN)
#define I2C_START (1 << TWINT) | (1 << TWEA)  | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE)
#define I2C_SEND  (1 << TWINT) | (1 << TWEA)  | (1 << TWEN) | (1 << TWIE)
#define I2C_STOP  (1 << TWINT) | (1 << TWSTO) | (1 << TWEN)
#define I2C_ACK   (1 << TWINT) | (1 << TWEA)  | (1 << TWEN) | (1 << TWIE)
#define I2C_NACK  (1 << TWINT) | (1 << TWEN)  | (1 << TWIE)

static uint8_t  twi_addr = 0;
static uint8_t *twi_data = 0;
static uint8_t  twi_len = 0;


void i2c_master_init() {
    PORTC |= (1 << PC4) | (1 << PC5); // Enable internal pull-ups. Need 4.7k-10k external resistors.
    TWSR = 0x00;
    TWBR = (F_CPU / I2C_MASTER_FREQ - 16) / 2;
}


uint8_t i2c_master_io(const uint8_t addr, uint8_t *data, const uint8_t len) {
    power_twi_enable();
    twi_addr = addr;
    twi_data = data;
    twi_len = len;
    uint32_t startMs = millis2();
    TWCR = I2C_START;
    while (TWCR & (1 << TWIE)) {
        if ((millis2() - startMs) > I2C_MASTER_TOT_MS) {
            power_twi_disable();
            return 1;
        }
    };
    power_twi_disable();
    return 0;
}

int8_t i2c_write(const uint8_t *buf, uint8_t num_bytes, uint8_t addr)
{
    //printf_P(PSTR("\ni2c_write(buf=%s, uint32_t num_bytes=%u, uint16_t addr=%u)\n"), buf, num_bytes, addr);
    if (i2c_master_io(TW_WRITE | (addr << 1), (uint8_t *)buf, num_bytes) != 0) return -EIO;
    return 0; // 0 - successful, or -EIO General - input / output error.
}

int8_t i2c_read(uint8_t *buf, uint8_t num_bytes, uint8_t addr)
{
    //printf_P(PSTR("\ni2c_read(buf=%s, uint32_t num_bytes=%u, uint16_t addr=%u)\n"), buf, num_bytes, addr);
    if (i2c_master_io(TW_READ | (addr << 1), buf, num_bytes) != 0) return -EIO;
    return 0;
}


ISR(TWI_vect) {
    switch (TWSR & TW_STATUS_MASK) {
        case TW_START:
        case TW_REP_START:
            TWDR = twi_addr;
            TWCR = I2C_SEND;
            break;
            
            // MASTER READ
        case TW_MR_DATA_ACK:
            *twi_data++ = TWDR;
            --twi_len;
            __attribute__ ((fallthrough));
        case TW_MR_SLA_ACK:
            TWCR = (twi_len > 1) ? I2C_ACK : I2C_NACK;
            break;
            
        case TW_MR_DATA_NACK:
            *twi_data = TWDR;
            --twi_len;
            __attribute__ ((fallthrough));
        case TW_MR_SLA_NACK:
            TWCR = I2C_STOP;
            break;
            
            // MASTER WRITE
        case TW_MT_SLA_ACK:
        case TW_MT_DATA_ACK:
            if (twi_len) {
                --twi_len;
                TWDR = *twi_data++;
                TWCR = I2C_SEND;
            } else {
                TWCR = I2C_STOP;
                while (TWCR & (1 << TWSTO)) {
                }
            }
            break;
            
        case TW_MT_SLA_NACK:
        case TW_MT_DATA_NACK:
            TWCR = I2C_STOP;
            while (TWCR & (1 << TWSTO)) {
            }
            break;
            
        case TW_MT_ARB_LOST:  // Same as TW_MR_ARB_LOST
            TWCR = I2C_START;
            break;
            
        default:
            TWCR = I2C_RESET;
            break;
    }
}


void leds_init() {
    DRIVER(PIN_LED_CHG, OUT);
    DRIVER(PIN_LED_DIS, OUT);
    OFF(PIN_LED_CHG);
    OFF(PIN_LED_DIS);
}

void leds_chg_set(bool on) {
    if (on) { ON(PIN_LED_CHG); } else {OFF(PIN_LED_CHG);  }
}

void leds_dis_set(bool on) {
    if (on) { ON(PIN_LED_DIS); } else {OFF(PIN_LED_DIS);  }
}

#endif
