#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#define UART_BAUDERATE 115200
#define UBRRN ((F_CPU / (8 * UART_BAUDERATE)) - 1)

/*
** Hexadecimal color representation string buffers
** Global variables to store received hex color codes
*/
char    g_hex_buffer[9]; // #RRGGBB + \r + \0 (total 9 bytes)
uint8_t g_buffer_index = 0;

void uart_puts(const char *str);

void init_rgb()
{
    DDRD |= (1 << DDD3) | (1 << DDD5) | (1 << DDD6); /* Set out mode */

    TCCR0A |= (1 << COM0A1) | (1 << COM0B1); /* page 113 15-3 / page 114 15-6  Setting pwm mode (Non-inverting PWM) */
    TCCR0A |= (1 << WGM01) | (1 << WGM00);  /* page 115 - 15-8 mode 3 top 255 (counter 0 to 255 (rgb value)) */

    TCCR0B |= (1 << CS00);  /* Do not prescaling (to avoid Flicker) page 116 */

    /*
    ** Timer 2 setting
    ** D3 pin connect with timer2
    */
    TCCR2A |= (1 << COM2B1); /* page 163 18-6 set OC2B at bottom*/

    TCCR2A |= (1 << WGM21) | (1 << WGM20);  /* page 164 18-8 model 3 */
    
    /*
    ** page 165 sheet of TCCR2B
    */
    TCCR2B |= (1 << CS20); /* Do not prescaling page 165 */

    /*
    ** OCR - Output Compare Register (page 108 15.7.3)
    */
    OCR0B = 0;  /* Red */
    OCR0A = 0;  /* Green */
    OCR2B = 0;  /* Blue */
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    OCR0B = r;  /* Red */
    OCR0A = g;  /* Green */
    OCR2B = b;  /* Blue */
}

/*
** UART Initialization Function (Setting Baudrate, Frame format, Enable RX/TX) 
** We need to serialize #RRGGBB format
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

uint8_t hex_char_to_int(char c)
{
    if (c >= '0' && c <= '9')
    {
        return (c - '0');
    }
    else if (c >= 'A' && c <= 'F')
    {
        return (c - 'A' + 10);
    }
    else if (c >= 'a' && c <= 'f')
    {
        return (c - 'a' + 10);
    }
    return (0);
}

/*
** Convert hex string to decimal integer (format #RRGGBB -> R, G, B values (divide by high and low))
*/
uint8_t convert_to_dec(char *str)
{
    uint8_t high;
    uint8_t low;
    uint8_t result;

    high = hex_char_to_int(str[0]);
    low = hex_char_to_int(str[1]);

    /*
    ** - 1 << 1 = 2 (1 * 2)
    ** - 1 << 2 = 4 (1 * 4)
    ** - 1 << 3 = 8 (1 * 8)
    ** - 1 << 4 = 16 (1 * 16)
    ** EXAMPLE:
    **    11110000  (high << 4)
    **    00000111  (low)
    **   ------------
    **   11110111  (Total 8 bit value)
    */
    result = (high << 4) | low;

    return result;
}

void hex_to_rgb(void)
{
    uint8_t r;
    uint8_t g;
    uint8_t b;

    r = convert_to_dec(&g_hex_buffer[1]); /* Skip '#' character */
    g = convert_to_dec(&g_hex_buffer[3]);
    b = convert_to_dec(&g_hex_buffer[5]);

    set_rgb(r, g, b);

    /* Send success message */
    uart_puts("\r\nColor displayed\r\n");
}

/*
** Check if the received string is in valid #RRGGBB format
** Return 1 if valid, 0 if invalid
** checking with 16
*/
int check_color_format(char c, uint8_t index)
{
    if (c != '#' && index == 0)
        return 0;

    if (index >= 1 && index <= 6)
    {
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'F') ||
            (c >= 'a' && c <= 'f'))
        {
            return 1;
        }
        else
            return 0;
    }
    
    if (index == 7 && c != '\r')
        return 0;
    return 1;
}

void uart_puts(const char *str)
{
    while (*str)
    {
        uart_tx(*str++);
    }
}

/*
** Interrupt Service Routine for receiving data
** USART_RX_vect - Receive controller
** ISR - Interrupt Service Routine (Stop while(1) and jump to this function)
*/
ISR(USART_RX_vect)
{
    char c = UDR0; /* Get and store the received data from UDR0 register */

    if (!check_color_format(c, g_buffer_index))
    {
        uart_puts("\r\nInvalid format!\r\n");

        g_buffer_index = 0; /* Reset index for next reception */
        
        return;
    }

    uart_tx(c); /* Echo back the received character */

    g_hex_buffer[g_buffer_index] = c; /* Store received character in buffer and increment index */

    if (g_buffer_index == 7) /* If we have received 8 characters (#RRGGBB\r) */
    {
        g_hex_buffer[8] = '\0'; /* Null-terminate the string */

        hex_to_rgb(); /* Convert hex to RGB and set the color */

        g_buffer_index = 0; /* Reset index for next reception */
    }
    else
    {
        g_buffer_index++;
    }
    
    
    
}


int main(void)
{
    uart_init(UBRRN); /* Initialize UART with calculated UBRR value */
    init_rgb();

    UCSR0B |= (1 << RXCIE0); /* Enable RX Complete Interrupt */

    sei();          /* Enable global interrupts */

    while (1)
    {

    }
}