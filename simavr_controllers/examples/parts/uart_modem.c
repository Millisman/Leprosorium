#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>

#include "uart_modem.h"
#include "avr_uart.h"
#include "sim_hex.h"
#include "sim_time.h"


DEFINE_FIFO(uint8_t, uart_modem_fifo);

// called when a byte is send via the uart on the AVR
static void uart_modem_in_hook(struct avr_irq_t * irq, uint32_t value, void * param) {
    uart_modem_t * p = (uart_modem_t*)param;
    // printf("uart_modem_in_hook %02x\n", value);
    uart_modem_fifo_write(&p->in, value);
}

// try to empty our fifo, the uart_pty_xoff_hook() will be called when other side is full
static void uart_modem_flush_incoming(uart_modem_t * p) {
        while (p->xon && !uart_modem_fifo_isempty(&p->out)) {
            uint8_t byte = uart_modem_fifo_read(&p->out);
            avr_raise_irq(p->irq + IRQ_UART_MODEM_BYTE_OUT, byte);
        }

}

avr_cycle_count_t uart_modem_flush_timer(
    struct avr_t * avr,
    avr_cycle_count_t when,
    void * param
) {
    uart_modem_t *p = (uart_modem_t*)param;
    uart_modem_flush_incoming(p);
    // always return a cycle NUMBER not a cycle count
    return p->xon ? when + avr_hz_to_cycles(p->avr, 1000) : 0;
}


// Called when the uart has room in it's input buffer. This is called repeateadly
// if necessary, while the xoff is called only when the uart fifo is FULL
static void uart_modem_xon_hook(struct avr_irq_t *irq, uint32_t value, void *param) {
    uart_modem_t * p = (uart_modem_t*)param;
    p->xon = 1;
    uart_modem_flush_incoming(p);
    // if the buffer is not flushed, try to do it later
    if (p->xon) {
        avr_cycle_timer_register(
            p->avr,
            avr_hz_to_cycles(p->avr, 1000),
            uart_modem_flush_timer, param
        );
    }
}

// Called when the uart ran out of room in it's input buffer
static void uart_modem_xoff_hook(struct avr_irq_t * irq, uint32_t value, void * param) {
    uart_modem_t * p = (uart_modem_t*)param;
    // if (p->xon) printf("uart_modem_xoff_hook\n");
    p->xon = 0;
}


static void uart_modem_to_avr(uart_modem_t *p, const char *data) {
    if (data) {
        uint16_t count = strlen(data);
        while (count-- && !uart_modem_fifo_isfull(&p->out)) {
            uint8_t c = *data++;
            uart_modem_fifo_write(&p->out, c);
        }
    }
}

static void *uart_modem_thread(void * param) {
    uart_modem_t *p = (uart_modem_t*)param;

    while (1) {
        bool echo = true;
        usleep(5000);

        uint8_t RxBuff[512];
        uint16_t RxCount = 0;

        while (!uart_modem_fifo_isempty(&p->in)) {

            RxBuff[RxCount] = uart_modem_fifo_read(&p->in);
            if (echo) { uart_modem_fifo_write(&p->out, RxBuff[RxCount]); }

            if (RxCount < sizeof(RxBuff)) {
                ++RxCount;
                RxBuff[RxCount] = '\0';
            } else {
                printf("Load Rx ovf!\n");
                RxCount = 0;
            }

            if (RxCount > 1 && RxBuff[RxCount-1] == '\n' && RxBuff[RxCount-2] == '\r') { // EOL

                if ( RxCount == 4 && strstr((char *)&RxBuff, "AT")) {
                    printf("AT detect!\n");
                    uart_modem_to_avr(p, "OK\r\n");
                    RxCount = 0;
                }

                if ( RxCount == 5 && strstr((char *)&RxBuff, "ATI")) {
                    printf("ATI detect!\n");
                    uart_modem_to_avr(p, "VirtModem v0.1\r\nOK\r\n");
                    RxCount = 0;
                }

                if ( RxCount == 9 && strstr((char *)&RxBuff, "AT+GCAP")) {
                    printf("AT+GCAP detect!\n");
                    uart_modem_to_avr(p, "+CAP1+CAP2+CAP3\r\nOK\r\n");
                    RxCount = 0;
                }

            } // EOL
        } // while !isempty
    } // loop
    return NULL;
}

static const char * irq_names[IRQ_UART_MODEM_COUNT] = {
    [IRQ_UART_MODEM_BYTE_IN] = "8<uart_modem.in",
    [IRQ_UART_MODEM_BYTE_OUT] = "8>uart_modem.out",
};

void uart_modem_init(struct avr_t * avr, uart_modem_t * p) {
    memset(p, 0, sizeof(*p));
    p->avr = avr;
    p->irq = avr_alloc_irq(&avr->irq_pool, 0, IRQ_UART_MODEM_COUNT, irq_names);
    avr_irq_register_notify(p->irq + IRQ_UART_MODEM_BYTE_IN, uart_modem_in_hook, p);
    printf("uart_modem_init - Virtual modem device\n");
    pthread_create(&p->thread, NULL, uart_modem_thread, p);
}

void uart_modem_connect(uart_modem_t * p, char uart) {
    // disable the stdio dump, as we are sending binary there
    uint32_t f = 0;
    avr_ioctl(p->avr, AVR_IOCTL_UART_GET_FLAGS(uart), &f);
    f &= ~AVR_UART_FLAG_STDIO;
    avr_ioctl(p->avr, AVR_IOCTL_UART_SET_FLAGS(uart), &f);

    avr_irq_t *src  = avr_io_getirq(p->avr, AVR_IOCTL_UART_GETIRQ(uart), UART_IRQ_OUTPUT);
    avr_irq_t *dst  = avr_io_getirq(p->avr, AVR_IOCTL_UART_GETIRQ(uart), UART_IRQ_INPUT);
    avr_irq_t *xon  = avr_io_getirq(p->avr, AVR_IOCTL_UART_GETIRQ(uart), UART_IRQ_OUT_XON);
    avr_irq_t *xoff = avr_io_getirq(p->avr, AVR_IOCTL_UART_GETIRQ(uart), UART_IRQ_OUT_XOFF);
    if (src && dst) {
        avr_connect_irq(src, p->irq + IRQ_UART_MODEM_BYTE_IN);
        avr_connect_irq(p->irq + IRQ_UART_MODEM_BYTE_OUT, dst);
    }
    if (xon)  avr_irq_register_notify(xon,  uart_modem_xon_hook, p);
    if (xoff) avr_irq_register_notify(xoff, uart_modem_xoff_hook, p);
}

