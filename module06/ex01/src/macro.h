#ifndef MACRO_H
#define MACRO_H

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#define UART_BAUDERATE 115200
#define UBRRN ((F_CPU / (8 * UART_BAUDERATE)) - 1)
#define I2C_ADDRESS_AHT20 (0x38 << 1) /* 7-bit address + Write/Read bit (0) so we need to shift left by 1 */
#define MEASUREMENT_CMD 0xAC

#endif /* MACRO_H */
