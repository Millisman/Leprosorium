#if !defined (F_CPU)
#define  F_CPU 4000000
#endif

#if !defined (__AVR_AT90S2313__)
#define  __AVR_AT90S2313__
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define SETBIT(x,y)   ((x) |= (1<<(y)))
#define CLEARBIT(x,y) ((x) &= (~(1<<(y))))
#define CHECKBIT(x,y) ((x) & (1<<(y)))


void delay(int t)
{
    for (int x=t; x>0 ;x--) __asm__ ("nop");
}

volatile uint8_t ir = 0;


ISR (INT1_vect) {
    ir++;
}
 
ISR (INT0_vect) {
    ir++;
}

int main(void) {
    DDRB = 0xFF; //all out
    DDRD = 0x00; //all in
 
    GIMSK=0b11000000;    //enable  int0 & int1
    // MCUCR=0b00001111;// int by rising front -  for button 1 & 2
    MCUCR=0b00001010;// int by rising front -  for button  & 2

    sei();
 
    while (1) {
        PORTB = ir;
    }
}










