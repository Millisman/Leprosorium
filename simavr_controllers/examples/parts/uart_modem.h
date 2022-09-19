#ifndef __UART_MODED_H___
#define __UART_MODED_H___


#include "sim_irq.h"
#include "fifo_declare.h"

enum {
    IRQ_UART_MODEM_BYTE_IN = 0,
    IRQ_UART_MODEM_BYTE_OUT,
    IRQ_UART_MODEM_COUNT
};

DECLARE_FIFO(uint8_t,uart_modem_fifo, 512);

typedef struct uart_modem_t {
    avr_irq_t           *irq;   // irq list
    struct avr_t        *avr;   // keep it around so we can pause it
    pthread_t           thread;
    int                 xon;
    uart_modem_fifo_t   in;
    uart_modem_fifo_t   out;
} uart_modem_t;

void uart_modem_init(struct avr_t * avr, uart_modem_t * b);

void uart_modem_connect(uart_modem_t * p, char uart);

#endif /* __UART_MODED_H___ */
