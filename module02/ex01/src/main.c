#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#define UART_BAUDERATE 115200
#define UBRRN ((F_CPU / (8 * UART_BAUDERATE)) - 1)   /* UBRRN value (F_CPU / (8 * UART_BAUDERATE)) - 1 // document - p.182 20-1 */

void uart_init(unsigned int ubrr)
{
    /* Set baud rate */
    UBRR0H = (unsigned char)(ubrr >> 8);    /* Upper Rate part */
    UBRR0L = (unsigned char)(ubrr);         /* Lower Rate part */

    /*
    ** UCSR0B - USART Control and Status Register 0 B
    ** RXEN0 - Receiver Enable
    ** TXENO - Transmitter Enable
    */
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);  /* Enable Receiver and Transmitter */
    
    /*
    ** Set frame format: 8N1 (8 data bits, No Parity, 1 stop bit)
    ** UCSR0C - USART Control and Status Register 0 C
    **
    ** (3 << UCSZ00) sets UCSZ01=1 and UCSZ00=1. Combined with UCSZ02=0 (default in UCSR0B),
    ** this selects 8-bit character size.
    **
    ** UPM0n bits (Parity) are 0 by default -> No Parity.
    ** USBS0 bit (Stop Bit) is 0 by default -> 1 Stop Bit.
    */
    UCSR0C = (3 << UCSZ00);  /* Format to 8N1 (N - No parity(error checking), and Data is 8 bits) Define bits */

    /*
    ** UCSR0A - USART Control and Status Register 0 A
    ** U2X0 - USART Double Speed 0 (Data speed optimization)
    */
    UCSR0A |= (1 << U2X0); /* Enabling double transfer speed, and divide by two baudrate (Reduce Error Rate) */
}

void uart_tx(char c)
{
    /*
    ** UCSR0A - USART Control and Status Register 0 A
    ** UDRE0 - USART Data Register Empty 0
    */
    while (!(UCSR0A & (1 << UDRE0)))   /* Wait for empty transmit buffer */
    {
        ;
    }

    /* UDR0 - USART Data Register 0  (REAL BOX which put the data)*/
    UDR0 = c;                           /* Put data into buffer, sends the data */
}

void uart_printstr(const char* str)
{
    while (*str)
    {
        uart_tx(*str++);
    }
}

/*
** Interrupt Service Routine for receiving data
** TIMER1_COMPA_vect - Timer/Counter1 Compare Match A
** ISR - Interrupt Service Routine (Stop while(1) and jump to this function)
*/
ISR(TIMER1_COMPA_vect)
{
    uart_printstr("Hello World!\r\n");   /* Simply send the heart beat */
}

int	main(void)
{
	uart_init(UBRRN);

	TCCR1B |= (1 << WGM12);

	TIMSK1 |= (1 << OCIE1A );

	sei();  /* Accept interrupt request */

    /*
    ** Have to set 2sec...
    ** Using 1024 prescaler -> why? 256 cycle -> 115200 / 256 = 62500 --> 0.5HZ --> 62500 * 2 == 125000, BUT 16 bit max 65535
    */
	OCR1A = 31249;
	TCCR1B |= (1 << CS12) | (1 << CS10); /* 1024 prescaler 1 0 1 */

	while (1) 
    {
		;
	}
}