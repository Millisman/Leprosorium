#pragma once
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-attributes"

#include <stdint.h>

/** @brief  UART Baudrate Expression
 *  @param  BAUD baudrate in bps, e.g. 1200, 2400, 9600
 */
#define UART_BAUD_SELECT(BAUD) ((F_CPU+8UL*BAUD)/(16UL*BAUD)-1UL)

/** @brief  UART Baudrate Expression for ATmega double speed mode
 *  @param  BAUD baudrate in bps, e.g. 1200, 2400, 9600
 */
#define UART_BAUD_SELECT_DOUBLE_SPEED(BAUD) (((F_CPU+4UL*BAUD)/(8UL*BAUD)-1) | 0x8000)

extern void uart0_init(uint16_t baudrate);
extern uint8_t uart0_getc(void);
extern uint8_t uart0_peek(void);
extern uint8_t uart0_available(void);
extern void uart0_flush(void);

extern void uart0_putc(uint8_t data);
extern void uart0_puts(const char *s);
extern void uart0_puts_p(const char *progmem_s);


