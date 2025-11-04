#ifndef MACRO_H
#define MACRO_H

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#define UART_BAUDERATE 115200
#define UBRRN ((F_CPU / (8 * UART_BAUDERATE)) - 1)

#define LED_D1    PB0
#define LED_D2    PB1
#define LED_D3    PB2
#define LED_D4    PB4

#define RGB_RED   PD5
#define RGB_GREEN PD6
#define RGB_BLUE  PD3

#define SET_RGB_OUTPUTS()   DDRD |= (1 << RGB_RED) | (1 << RGB_GREEN) | (1 << RGB_BLUE)
#define PORT_ON() PORTB |= (1 << LED_D1) | (1 << LED_D2) | (1 << LED_D3) | (1 << LED_D4)
#define PORT_OFF() PORTB &= ~((1 << LED_D1) | (1 << LED_D2) | (1 << LED_D3) | (1 << LED_D4))

#endif