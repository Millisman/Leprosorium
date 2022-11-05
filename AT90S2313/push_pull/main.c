#if !defined (F_CPU)
#define  F_CPU 16000000UL
#endif

#if !defined (__AVR_ATmega328P__)
#define  __AVR_ATmega328P__
#endif

#if !defined (__AVR_ATmega328__)
#define  __AVR_ATmega328__
#endif




#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>


/** Set y bit in the byte x */
#define SETBIT(x,y)   ((x) |= (1<<(y)))

/** Clear y bit in the byte x */
#define CLEARBIT(x,y) ((x) &= (~(1<<(y))))

/** Check y bit in the byte x */
#define CHECKBIT(x,y) ((x) & (1<<(y)))

// This code demonstrates how to generate two output signals
// with variable phase shift between them using an AVR Timer 
// The output shows up on Arduino pin 9, 10
// More AVR Timer Tricks at http://josh.com
// http://electronics-diy.com/pwm-motor-driver-mosfet-h-bridge-avr-atmega8.php


void setup() {
    
    DDRB |= 1<<PINB1;
    DDRB |= 1<<PINB2;
    
    // Both outputs in toggle mode  
    TCCR1A = _BV( COM1A0 ) |_BV( COM1B0 );
   
    
    // CTC Waveform Generation Mode
    // TOP=ICR1  
    // Note clock is left off for now
    
    TCCR1B = _BV( WGM13) | _BV( WGM12);
    
    OCR1A = 0;    // First output is the base, it always toggles at 0
    
    
}

// prescaler of 1 will get us 8MHz - 488Hz
// User a higher prescaler for lower freqncies
// ???ы
// #define PRESCALER 1
// #define PRESCALER_BITS 0x01

// Output phase shifted wave forms on Arduino Pins 9 & 10
// freq = freqnecy in Hertz (  122 < freq <8000000 )
// shift = phase shift in degrees ( 0 <= shift < 180 )

// Do do shifts 180-360 degrees, you could invert the OCR1B by doing an extra toggle using FOC

/// Note phase shifts will be rounded down to the next neared possible value so the higher the frequency, the less phase shift resolution you get. At 8Mhz, you can only have 0 or 180 degrees because there are only 2 clock ticks per cycle.  

void setWaveforms( unsigned long freq , int shift ) {
    
    // This assumes prescaler = 1. For lower freqnecies, use a larger prescaler.
    
    unsigned long clocks_per_toggle = (F_CPU / freq) / 2;    // /2 becuase it takes 2 toggles to make a full wave
    
    ICR1 = clocks_per_toggle;
    
    unsigned long offset_clocks = (clocks_per_toggle * shift) / 180UL; // Do mult first to save precision
    
    OCR1B= offset_clocks;
    
    // Turn on timer now if is was not already on
    // Clock source = clkio/1 (no prescaling)
    // Note: you could use a prescaller here for lower freqnencies
    TCCR1B |= _BV( CS10 ); 
    
}

// Demo by cycling through some phase shifts at 50Khz  

int main(void) {
    
    setup();
    sei();
    
    while (1) {
    
        setWaveforms( 50000 , 0 );
        
        _delay_ms(1000);
        
//         delay(1000); 
        
        setWaveforms( 50000 , 90 );
        
        _delay_ms(1000);
//         delay(1000); 
        
        setWaveforms( 50000 , 180 );
        
        _delay_ms(1000);
//         delay(1000); 
    
    }
}

// int main ()   
// {
//     // 
//     //************************* Timer 0 *********************************
//     // wgm mode 111 (fast pwm w/ TOP = OCRA)
//     TCCR0A |=  1<<WGM00 | 1<<WGM01; 
//     TCCR0B |= 1<<WGM02; 
//     //set COM0x1 (non-inverting mode)
//     TCCR0A |=  1<<COM0A1 ; 
//     TCCR0A |=  1<<COM0B1 ; 
//     //pre-scaler = 1
//     TCCR0B |=  1<<CS00; 
// 
//     // arbitrary frequency
//     OCR0A  = 220; //counts until TCNT = OCR0A then resets
//     OCR0B = OCR0A/2; //on until TCNT = OCR0B then off
//     // turn on pin D5
//     DDRD |= 1<<PIND5;
//     TCNT2 += OCR0A/4 ; //add a 90 degree delay to TCNT2... or something like that
//    
//    
//     //**************************** Timer 2 ****************************************
//     // wgm mode 111 (phase-corrected pwm w/ TOP = OCRA)
//     TCCR2A |=  1<<WGM20 | 1<<WGM21; 
//     TCCR2B |= 1<<WGM22; 
//     //set COM0x1 (non-inverting mode)
//     TCCR2A |=  1<<COM2A1 ; 
//     TCCR2A |=  1<<COM2B1 ; 
//     //pre-scaler = 1
//     TCCR2B |=  1<<CS20; 
//     // same frequency as above pwm on timer 0
//     OCR2A  = OCR0A; 
//     OCR2B = OCR2A/2;
//     // turn on pin D3
//     DDRD |= 1<<PIND3;
// }
//     















