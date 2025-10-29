#ifndef EMB_H
# define EMB_H

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

typedef enum e_state
{
    STATE_WAIT_USERNAME,
    STATE_WAIT_PASSWORD,
    STATE_CHECKING,
    STATE_LOGGED_IN,
    STATE_ERROR
}   t_state;

#define F_CPU 16000000UL
#define UART_BAUDERATE 115200
#define UBRRN ((F_CPU / (8 * UART_BAUDERATE)) - 1)   /* UBRRN value (F_CPU / (8 * UART_BAUDERATE)) - 1 // document - p.182 20-1 */
#define SET_MODE(x, num) DDR##x |= (1 << DD##x##num)
#define PRINT_MODE(x, num) PORT##x |= (1 << PORT##x##num);

#endif