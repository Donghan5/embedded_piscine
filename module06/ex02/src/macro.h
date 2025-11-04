#ifndef MACRO_H
#define MACRO_H

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <stdlib.h>

#define F_CPU 16000000UL
#define UART_BAUDERATE 115200
#define UBRRN ((F_CPU / (8 * UART_BAUDERATE)) - 1)
#define I2C_ADDRESS_AHT20 (0x38 << 1) /* 7-bit address + Write/Read bit (0) so we need to shift left by 1 */
#define MEASUREMENT_CMD 0xAC
#define RESOLUTION 1048576.0
#define TEMPERATURE_SCALE 200.0
#define HUMIDITY_SCALE 100.0
#define OFFSET 50.0

#endif /* MACRO_H */
