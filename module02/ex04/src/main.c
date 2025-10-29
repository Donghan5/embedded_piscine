#include "emb.h"

char g_username_buffer[32];
char g_password_buffer[32];

int g_input_ready = 0;  /* 0: not complete / 1: completed */

volatile uint8_t g_buffer_index = 0;    /* To check index of string */

volatile t_state g_current_state = STATE_WAIT_USERNAME; /* To check current state of machine */

/*
* Wait 2~3 sec, beacase of reset
*/

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

void uart_puts(const char *str)
{
    while (*str)
    {
        uart_tx(*str++);
    }
}

char uart_rx(void)
{
    /*
    ** RXC0 - USART Receive Complete
    */
    while (!(UCSR0A & (1 << RXC0)))   /* Wait for receive buffer */
    {
        ;
    }

    return UDR0;
}

int	ft_strncmp(const char *s1, const char *s2, int n)
{
	int	i;

	i = 0;
	if (n == 0)
		return (0);
	while ((s1[i] != '\0' || s2[i] != '\0') && i < n)
	{
		if (s1[i] != s2[i])
		{
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		}
		i++;
	}
	return (0);
}

/*
** Interrupt Service Routine for receiving data
** USART_RX_vect - Receive controller
** ISR - Interrupt Service Routine (Stop while(1) and jump to this function)
*/
ISR(USART_RX_vect)
{
    char c = UDR0;

    if (c == '\r' || c == '\n') /* When user puts enter */
    {
        if (g_current_state == STATE_WAIT_USERNAME)
        {
            g_username_buffer[g_buffer_index] = '\0';   /* End string */
            g_current_state = STATE_WAIT_PASSWORD;
        }
        else if (g_current_state == STATE_WAIT_PASSWORD)
        {
            g_password_buffer[g_buffer_index] = '\0'; /* End string */
            g_current_state = STATE_CHECKING;
        }
        
        g_buffer_index = 0;
        g_input_ready = 1;
        uart_puts("\r\n");
    }
    else if (c == 127 || c == 8) /* BackSpace */
    {
        if (g_buffer_index > 0)
        {
            g_buffer_index--;
            uart_tx('\b');
            uart_tx(' ');
            uart_tx('\b');
        }
    }
    else
    {
        
        if (g_buffer_index < (32 - 1)) 
        {
            if (g_current_state == STATE_WAIT_USERNAME)
            {
                g_username_buffer[g_buffer_index] = c;
                uart_tx(c);
            }
            else if (g_current_state == STATE_WAIT_PASSWORD)
            {
                g_password_buffer[g_buffer_index] = c;
                uart_tx('*');
            }
            g_buffer_index++;
        }
    }
}

int main(void)
{
    const char *correct_user = "spectre";
    const char *correct_pass = "spectre";

    uart_init(UBRRN);
    UCSR0B |= (1 << RXCIE0);    /* Enable the RX Complete Interrupt */
    sei();

    SET_MODE(B, 0);

    uart_puts("Username: ");

    while (1)
        {
            if (g_input_ready == 1)
            {
                g_input_ready = 0;  /* Flag reset */

                if (g_current_state == STATE_WAIT_PASSWORD)
                {
                    uart_puts("Password: ");
                }
                else if (g_current_state == STATE_CHECKING)
                {
                    if (ft_strncmp(g_username_buffer, correct_user, 32) == 0 &&
                        ft_strncmp(g_password_buffer, correct_pass, 32) == 0)
                    {
                        g_current_state = STATE_LOGGED_IN;
                        
                        uart_puts("Hello ");
                        uart_puts(correct_user);
                        uart_puts("\r\n");
                        uart_puts("Shall we play a game?\r\n");
                    }
                    else
                    {
                        uart_puts("\r\nBad combinaison username/password\r\n");
                        g_current_state = STATE_WAIT_USERNAME;
                        uart_puts("Username: ");
                    }
                }
            }

            if (g_current_state == STATE_LOGGED_IN)
            {
                PRINT_MODE(B, 0);
            }
        }
}
