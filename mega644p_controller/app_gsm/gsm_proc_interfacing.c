#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <string.h>

#include "gsm_proc.h"
#include "mcu/mcu.h"

volatile ModemTxBuff_t __mdm_tx;
volatile ModemRxBuff_t __mdm_rx;

void _hal_modem_init_port() {
    memset((void *)&__mdm_tx, 0, sizeof(__mdm_tx));
    memset((void *)&__mdm_rx, 0, sizeof(__mdm_rx));


    UCSR1A = 1 << U2X1;
    UBRR1H = ((F_CPU / 4 / MODEM_BAUD_RATE - 1) / 2) >> 8;
    UBRR1L = ((F_CPU / 4 / MODEM_BAUD_RATE - 1) / 2) & 0xFF;
    UCSR1B = ((1<<TXEN1) | (1<<RXEN1) | (1<<RXCIE1) | (1<<TXCIE1));
}

ISR(USART1_TX_vect) {
    if (__mdm_tx.tx_buff_head != __mdm_tx.tx_buff_tail) {
        uint8_t c = __mdm_tx.tx_buff[__mdm_tx.tx_buff_tail];
        if (++__mdm_tx.tx_buff_tail >= MODEM_TX_BUFF_SIZE) __mdm_tx.tx_buff_tail = 0;
        UDR1 = c;
    }
}

volatile uint8_t __rxc1;

ISR(USART1_RX_vect) {
    if (bit_is_set(UCSR1A, UPE1)) {
        __rxc1 = UDR1; // parity error! read
    } else {
        __rxc1 = UDR1;

        if ( __rxc1 == '\n' && __mdm_rx.rx_len > 0 && __mdm_rx.rx_buff[__mdm_rx.rx_len-1] == '\r') {

            if (__mdm_rx.rx_len == 3 &&
                __mdm_rx.rx_buff[0] == 'O' &&
                __mdm_rx.rx_buff[1] == 'K'
            ) {
                __mdm_rx.trip_ok = true;
            } else if (__mdm_rx.rx_len == 6 &&
                __mdm_rx.rx_buff[0] == 'E' &&
                __mdm_rx.rx_buff[1] == 'R' &&
                __mdm_rx.rx_buff[2] == 'R' &&
                __mdm_rx.rx_buff[3] == 'O' &&
                __mdm_rx.rx_buff[4] == 'R'
            ) {
                __mdm_rx.trip_err = true;
            } else {
                __mdm_rx.rx_buff[__mdm_rx.rx_len] = 0;
                if (__mdm_rx.rx_len > 2) {
                    if (__mdm_rx.save_size && __mdm_rx.save_ptr) {
                        if (__mdm_rx.save_size > __mdm_rx.rx_len) { __mdm_rx.save_size = __mdm_rx.rx_len; }
                        strncpy(__mdm_rx.save_ptr, (const void *)&__mdm_rx.rx_buff, __mdm_rx.save_size);
                        __mdm_rx.save_ptr = 0;
                        __mdm_rx.save_size = 0;
                    } else {
                        __mdm_rx.trip_scan = true;
                        __mdm_rx.rx_len_scan = __mdm_rx.rx_len;
                    }
                }
            }
            __mdm_rx.rx_len = 0;
        } else {
            __mdm_rx.rx_buff[__mdm_rx.rx_len] = __rxc1;
            if (__mdm_rx.rx_len < MODEM_RX_BUFF_SIZE) { ++__mdm_rx.rx_len; }
        }
        if (__mdm_rx.rx_len == 2 &&
            __mdm_rx.rx_buff[0] == '>' &&
            __mdm_rx.rx_buff[1] == ' '
        ) {
            __mdm_rx.trip_prompt = true;
            __mdm_rx.rx_len = 0;
        }
    }
}

void _hal_modem_write(const uint8_t c) {
    // ------------- direct write ------------
    // while (!(UCSR1A & (1<<UDRE1)));
    // UDR1 = data;
    // ------------- buffer write ------------
    uint16_t i = (uint16_t)(__mdm_tx.tx_buff_head + 1 >= MODEM_TX_BUFF_SIZE) ? 0 : __mdm_tx.tx_buff_head + 1;
    while ( (i + 1) == __mdm_tx.tx_buff_tail);
    if (i != __mdm_tx.tx_buff_tail) {
        __mdm_tx.tx_buff[__mdm_tx.tx_buff_head] = c;
        __mdm_tx.tx_buff_head = i;
    }

    // ---------- start Transmission ---------
    while (!(UCSR1A & (1<<UDRE1)));
    if (__mdm_tx.tx_buff_head != __mdm_tx.tx_buff_tail) {
        uint8_t c = __mdm_tx.tx_buff[__mdm_tx.tx_buff_tail];
        if (++__mdm_tx.tx_buff_tail >= MODEM_TX_BUFF_SIZE) __mdm_tx.tx_buff_tail = 0;
        UDR1 = c;
    }

}
