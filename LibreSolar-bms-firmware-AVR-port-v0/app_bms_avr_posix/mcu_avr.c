#include "mcu.h"

#if defined (__AVR__) || defined (__AVR_ARCH__)

#include "uart.h"
#include "errno-base.h"
#include <util/twi.h>

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define MICROSECONDS_PER_TIMER2_OVERFLOW    (clockCyclesToMicroseconds(64 * 256))
#define MILLIS2_INC     (MICROSECONDS_PER_TIMER2_OVERFLOW / 1000)
#define FRACT2_INC      ((MICROSECONDS_PER_TIMER2_OVERFLOW % 1000) >> 3)
#define FRACT2_MAX      (1000 >> 3)
#define MICROS2_MULT    (64 / clockCyclesPerMicrosecond())

volatile uint32_t   timer2_millis   = 0;
volatile uint32_t   timer2_uptime_s = 0;
static uint8_t      timer2_fract    = 0;
static uint16_t     timer2_kilovar  = 0;

ISR(TIMER2_OVF_vect) {
    timer2_millis += MILLIS2_INC;
    timer2_fract += FRACT2_INC;
    if (timer2_fract >= FRACT2_MAX) {
        timer2_fract -= FRACT2_MAX;
        timer2_millis++;
        timer2_kilovar++;
        if (timer2_kilovar > 999) {
            timer2_uptime_s++;
            timer2_kilovar = 0;
        }
    }
}

uint32_t millis2() {
    uint8_t oldSREG = SREG;
    cli();
    uint32_t m = timer2_millis;
    SREG = oldSREG;
    return m;
}

uint32_t uptime2() {
    uint8_t oldSREG = SREG;
    cli();
    uint32_t m = timer2_uptime_s;
    SREG = oldSREG;
    return m;
}


extern bool     alert_interrupt_flag;
extern time_t   alert_interrupt_timestamp;

ISR(INT0_vect) {
    alert_interrupt_timestamp = uptime2();
    alert_interrupt_flag = true;
}

extern uint32_t time_pressed;
ISR(INT1_vect) {
    time_pressed = millis2();
}



int uart0_putc2(char ch, FILE *stream) {
    if (ch == '\n') uart0_putc2('\r', stream);
    uart0_putc(ch);
    return 0;
}


// int
// uart_putchar(char c)
// {
//     
// //     if (c == '\n')
// //         uart_putchar('\r');
// //     loop_until_bit_is_set(UCSRA, UDRE);
// //     UDR = c;
//     return 0;
// }



#define CHANGE  1
#define FALLING 2
#define RISING  3

static FILE uart0_file;

void mcu_init() {
    fdevopen(uart0_putc2, NULL);
        // // //     fdev_setup_stream( &uart0_file, uart0_putc2, NULL, _FDEV_SETUP_WRITE);
        // // //     uart0_init(UART_BAUD_SELECT(115200, F_CPU));
        // // //     stdout = &uart0_file;
    printf_P(PSTR("mcu_init\n"));
    
    
    // -------- Timer2 init for millis2
    TCCR2A = (1 << WGM20) | (1 << WGM21);
    TCCR2B = 1 << CS22;
    TIMSK2 = 1 << TOIE2;

    // -------- Int0 init - ALERT
    EICRA = (1 << ISC01) | (1 << ISC00) | (RISING << ISC00);
    EIMSK = (1 << INT0);

    // -------- Int1 init - BUTTON
    EICRA = (1 << ISC11) | (1 << ISC10) | (FALLING << ISC10);
    EIMSK = (1 << INT1);
    
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
    twi_addr = addr;
    twi_data = data;
    twi_len = len;
    uint32_t startMs = millis2();
    TWCR = I2C_START;
    while (TWCR & (1 << TWIE)) {
        if ((millis2() - startMs) > I2C_MASTER_TOT_MS) {
            return 1;
        }
    };
    return 0;
}



int8_t i2c_write(const uint8_t *buf, uint8_t num_bytes, uint8_t addr)
{
    printf_P(PSTR("\ni2c_write(buf=%s, uint32_t num_bytes=%u, uint16_t addr=%u)\n"), buf, num_bytes, addr);
    if (i2c_master_io(TW_WRITE | (addr << 1), (uint8_t *)buf, num_bytes) != 0) return -EIO;
    return 0; // 0 - successful, or -EIO General - input / output error.
}

int8_t i2c_read(uint8_t *buf, uint8_t num_bytes, uint8_t addr)
{
    printf_P(PSTR("\ni2c_read(buf=%s, uint32_t num_bytes=%u, uint16_t addr=%u)\n"), buf, num_bytes, addr);
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
