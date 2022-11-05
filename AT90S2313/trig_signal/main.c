#if !defined (F_CPU)
#define  F_CPU 4000000
#endif

#if !defined (__AVR_AT90S2313__)
#define  __AVR_AT90S2313__
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

/** Set y bit in the byte x */
#define SETBIT(x,y)   ((x) |= (1<<(y)))

/** Clear y bit in the byte x */
#define CLEARBIT(x,y) ((x) &= (~(1<<(y))))

/** Check y bit in the byte x */
#define CHECKBIT(x,y) ((x) & (1<<(y)))



#define A_0 CLEARBIT(PORTB, PB4)
#define A_1   SETBIT(PORTB, PB4)
#define B_0 CLEARBIT(PORTB, PB3)
#define B_1   SETBIT(PORTB, PB3)
#define I_I   CHECKBIT(PIND, PD5)


int main(void)
{
     DDRD  = 0b00000000; // All input
//     PORTD = 0b00000000; // pullup on pins

    DDRB  = 0b00011000; // PB3, PB4 - out
    PORTB = 0b00000000; // pulldowns on pins

//    TCCR0 |= (1 << CS02);   // set up timer with prescaler = 256
//    TCCR0 |= (1 << CS00);   // set up timer with prescaler = 0
//    TCNT0 = 0;              // initialize counter
//    TIMSK |= (1 << TOIE0);  // enable overflow interrupt
//    sei();                  // enable global interrupts

    A_0;
    B_0;
    
    uint32_t count = 0;
    while(1) {
        if (I_I) { // Ask A = 1
            if (!CHECKBIT(PINB, PB4)) { // not active
                    A_0;
                    B_0;
                 if (count > 3200000) {
                     count = 0;
                     A_1;
                 } else {
                     count++;
                     B_0;
                 }
            }
            
        } else { // Ask B = 1
            
             if (!CHECKBIT(PINB, PB3)) { // not active
                 A_0;
                 B_0;    
                 if (count > 3200000) {
                     count = 0;
                     B_1;
                 } else {
                     count++;
                     A_0;
                 }
             }
        }


    }

}
