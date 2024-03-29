#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>

volatile uint8_t _usart0_rx_buffer[32] = {0};
volatile uint8_t _usart0_rx_index_insert = 0;
volatile uint8_t _usart0_rx_index_read = 0;

volatile uint8_t _usart0_tx_buffer[128] = {0};
volatile uint8_t _usart0_tx_index_insert = 0;
volatile uint8_t _usart0_tx_index_read = 0;

volatile uint8_t _usart0_data_io;

ISR(USART0_RX_vect) {
    if (UCSR0A & (_BV(FE0) | _BV(DOR0))) {
        UDR0;
        // Framing Error by UART
        // Overrun condition by UART
    } else {
        _usart0_rx_buffer[_usart0_rx_index_insert++] = UDR0;
        if (_usart0_rx_index_insert >= sizeof(_usart0_rx_buffer)) _usart0_rx_index_insert = 0;
        if (_usart0_rx_index_insert == _usart0_rx_index_read) ++_usart0_rx_index_read;
    }
}

ISR(USART0_UDRE_vect) {
    if (_usart0_tx_index_read >= sizeof(_usart0_tx_buffer)) _usart0_tx_index_read = 0;
    if (_usart0_tx_index_read == _usart0_tx_index_insert) {
        UCSR0B &= ~_BV(UDRIE0);
    } else {
        UDR0 = _usart0_tx_buffer[_usart0_tx_index_read++];
    }
}

void uart0_init(uint16_t baudrate) {
    UCSR0B &= ~_BV(RXCIE0); // disable RXCE interrupt
    UCSR0B &= ~_BV(UDRIE0); // disable UDRE interrupt
    // Set baud rate, 2x speed ?
    if (baudrate & 0x8000) {
        UCSR0A = _BV(U2X0);
        baudrate &= ~0x8000;
    }
    UBRR0H = (uint8_t)(baudrate >> 8);
    UBRR0L = (uint8_t) baudrate;
    // Enable USART receiver and transmitter and receive complete interrupt
    UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
    UCSR0C = (3 << UCSZ00);
}

uint8_t uart0_getc(void) {
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        if (_usart0_rx_index_read >= sizeof(_usart0_rx_buffer)) _usart0_rx_index_read = 0;
        if (_usart0_rx_index_read == _usart0_rx_index_insert) {
            _usart0_data_io = 0;
        } else {
            _usart0_data_io = _usart0_rx_buffer[_usart0_rx_index_read++];
        }
    }
    return _usart0_data_io;
}

uint8_t uart0_peek(void) {
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        if (_usart0_rx_index_read >= sizeof(_usart0_rx_buffer)) {
            _usart0_rx_index_read = 0;
        }
        if (_usart0_rx_index_read == _usart0_rx_index_insert) _usart0_data_io = 0;
        _usart0_data_io = _usart0_rx_buffer[_usart0_rx_index_read];
    }
    return _usart0_data_io;
}

uint8_t uart0_available(void) {
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        _usart0_data_io = _usart0_rx_index_insert - _usart0_rx_index_read;
    }
    return _usart0_data_io;
}

void uart0_flush(void) {
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        _usart0_rx_index_insert = _usart0_rx_index_read;
    }
}

void uart0_putc(uint8_t data) {

    _usart0_tx_buffer[_usart0_tx_index_insert++] = data;
    if (_usart0_tx_index_insert >= sizeof(_usart0_tx_buffer)) _usart0_tx_index_insert = 0;
    if (_usart0_tx_index_insert == _usart0_tx_index_read) ++_usart0_tx_index_read;
    UCSR0B |= _BV(UDRIE0);
    while (_usart0_tx_index_insert - _usart0_tx_index_read >= sizeof(_usart0_tx_buffer));
}

void uart0_puts(const char *s) {
    while (*s) { uart0_putc(*s++); }
}

void uart0_puts_p(const char *progmem_s) {
    char c;
    while ((c = pgm_read_byte(progmem_s++))) { uart0_putc(c); }

}






