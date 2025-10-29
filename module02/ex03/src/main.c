#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#define UART_BAUDERATE 115200
#define UBRRN ((F_CPU / (8 * UART_BAUDERATE)) - 1)   /* UBRRN value (F_CPU / (8 * UART_BAUDERATE)) - 1 // document - p.182 20-1 */

int	ft_tolower(int c)
{
	if (c >= 'A' && c <= 'Z')
		return (c + 32);
	return (c);
}

int	ft_toupper(int c)
{
	if (c >= 'a' && c <= 'z')
		return (c - 32);
	return (c);
}

int ft_islower(int c)
{
    if (c >= 'a' && c <= 'z')
        return 0;
    return 1;
}

int ft_isupper(int c)
{
    if (c >= 'A' && c <= 'Z')
		return (0);
	return (1);
}

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
    UCSR0C = (3 << UCSZ00);  /* Format to 8N1 (N - No parity, and Data is 8 bits)*/

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

/*
** Interrupt Service Routine for receiving data
** USART_RX_vect - Receive controller
** ISR - Interrupt Service Routine (Stop while(1) and jump to this function)
*/
ISR(USART_RX_vect)
{
    char c = UDR0;

    if (ft_islower(c) == 0)
    {
        c = ft_toupper(c);
    }
    else if (ft_isupper(c) == 0)
    {
        c = ft_tolower(c);
    }

    uart_tx(c);
}

int main(void)
{
    uart_init(UBRRN);

    /*
    ** RXICE0 - Receive Complete Interrupt Enable 0
    */
    UCSR0B |= (1 << RXCIE0);    /* Enable the RX Complete Interrupt */
    
    sei();

    while (1)
    {
        ;
    }
}