#include "timer.h"


#if defined (__AVR__) || defined (__AVR_ARCH__)

static bool elapsed_1_sec;
static uint8_t  counter_10int = 0;
static uint16_t  counter_1000int = 0;
static uint32_t timer0_millis = 0;
static uint32_t timer0_uptime_sec = 0;

ISR(TIMER0_COMPA_vect) {
    counter_10int++;
    if (counter_10int >= 10) {
        counter_10int = 0;
        ++timer0_millis;
        ++counter_1000int;
        if (counter_1000int >= 1000) {
            counter_1000int = 0;
            ++timer0_uptime_sec;
            elapsed_1_sec = true;
        }
    }
}

namespace mcu {

void Timer::begin() {
    // http://www.8bit-era.cz/arduino-timer-interrupts-calculator.html
    // TIMER 0 for interrupt frequency 10 000 Hz:
    TCCR0A = 0; // set entire TCCR0A register to 0
    TCCR0B = 0; // same for TCCR0B
    TCNT0  = 0; // initialize counter value to 0
    // set compare match register for 10000 Hz increments
    OCR0A = 249; // = 20000000 / (8 * 10000) - 1 (must be <256)
    // turn on CTC mode
    TCCR0B |= (1 << WGM01);
    // Set CS02, CS01 and CS00 bits for 8 prescaler
    TCCR0B |= (0 << CS02) | (1 << CS01) | (0 << CS00);
    // enable timer compare interrupt
    TIMSK0 |= (1 << OCIE0A);
    sei(); // allow interrupts
    elapsed_1_sec = false;
}

uint32_t Timer::millis() {
    uint8_t old_SREG = SREG;
    cli();
    uint32_t ret = timer0_millis;
    SREG = old_SREG;
    return ret;
}

uint32_t Timer::uptime_sec() {
    uint8_t old_SREG = SREG;
    cli();
    uint32_t ret = timer0_uptime_sec;
    SREG = old_SREG;
    return ret;
}

bool Timer::elapsed_one_sec() {
    if (elapsed_1_sec) {
       elapsed_1_sec = false;
       return true;
    }
    return false;
}


}  // namespace mcu

#endif
