#include "serial.h"

#if defined (__AVR__) || defined (__AVR_ARCH__)

#define USART0_TX_BUFFER_SIZE 128

namespace {

static volatile uint8_t USART0_TX_BUFFER[USART0_TX_BUFFER_SIZE];
static volatile uint8_t USART0_TX_BUFFER_HEAD;
static volatile uint8_t USART0_TX_BUFFER_TAIL;

ISR (USART0_TX_vect) {
    if (USART0_TX_BUFFER_HEAD != USART0_TX_BUFFER_TAIL) {
        UDR0 = USART0_TX_BUFFER[USART0_TX_BUFFER_TAIL];
        if (++USART0_TX_BUFFER_TAIL >= USART0_TX_BUFFER_SIZE) USART0_TX_BUFFER_TAIL = 0;
    }
}

}

namespace mcu {

void SerOut::begin(uint16_t baud_setting) {
    USART0_TX_BUFFER_HEAD = 0;
    USART0_TX_BUFFER_TAIL = 0;
    UCSR0A = 1 << U2X1;
    UBRR0H = baud_setting >> 8;
    UBRR0L = baud_setting;
    UCSR0B = 1 << TXEN0  | 1<<TXCIE0;
    UCSR0C = 1 << UCSZ01 | 1<<UCSZ00;
}

void SerOut::write(const uint8_t data) {
    uint8_t i = (USART0_TX_BUFFER_HEAD + 1 >= USART0_TX_BUFFER_SIZE) ? 0 : USART0_TX_BUFFER_HEAD + 1;
    while ( (i + 1) == USART0_TX_BUFFER_TAIL);
    if (i != USART0_TX_BUFFER_TAIL) {
        USART0_TX_BUFFER[USART0_TX_BUFFER_HEAD] = data;
        USART0_TX_BUFFER_HEAD = i;
    }
    while (!(UCSR0A & (1<<UDRE0)));
    if (USART0_TX_BUFFER_HEAD != USART0_TX_BUFFER_TAIL) {
        UDR0 = USART0_TX_BUFFER[USART0_TX_BUFFER_TAIL];
        if (++USART0_TX_BUFFER_TAIL >= USART0_TX_BUFFER_SIZE) USART0_TX_BUFFER_TAIL = 0;
    }
}

}

#endif



