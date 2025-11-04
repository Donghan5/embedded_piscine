#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#define UART_BAUDERATE 115200
#define UBRRN ((F_CPU / (8 * UART_BAUDERATE)) - 1)

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

void uart_puts(char *str)
{
    while (*str)
    {
        uart_tx(*str++);
    }
}

void init_adc(void)
{
    /*
    ** ADC Initialization
    ** ADMUX - ADC Multiplexer Selection Register
    ** REFS0 - Reference Selection Bit 0 -> AVCC with external capacitor at AREF pin (page 257 24-3) Voltage Reference Selection
    **       - set in AVCC
    ** ADLAR - ADC Left Adjust Result (page 257) (for 8 bit resolution we need left adjust result) -< if 0, 10 bit resolution
    ** RESF0 - INPUT / ADLAR - OUTPUT
    */
    ADMUX = (1 << REFS0); /* AVcc with external capacitor at AREF pin */

    /*
    ** ADCSRA - ADC Control and Status Register A
    ** ADEN - ADC Enable
    ** ADPS2:0 - ADC Prescaler Select Bits (page 259 24-5)
    */
    ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); /* Enable ADC and set prescaler to 128 (16MHz/128 = 125KHz) (page 259 24-5) */
}

uint16_t read_adc_channel(uint8_t channel)
{
    /*
    ** We need to select the ADC channel by modifying the MUX bits in ADMUX register.
    ** MUX bits are the lowest 3 bits of ADMUX (MUX2:MUX0).
    ** 0xF8 = 0b11111000
    ** 0x07 = 0b00000111
    */
    ADMUX = (ADMUX & 0xF8) | (channel & 0x07); /* Select ADC channel, keep other bits intact */
    
    /*
    ** We initiate the ADLAR to 1 in init_adc() so we can read only the high byte (8 bits resolution)
    ** So we read only ADSC
    ** ADCSRA - ADC Control and Status Register A (page 258)
    ** ADSC - ADC Start Conversion
    */
    ADCSRA |= (1 << ADSC);               /* Start single conversion */

    while (ADCSRA & (1 << ADSC))       /* Wait for conversion to complete */
    {
        ;
    }

    uint8_t low = ADCL;
    uint8_t high = ADCH;
    
    return ((uint16_t)high << 8) | low; // 10-bit result to uint16_t
}

/*
** Format a byte value as a decimal string.
*/
void format_dec(uint16_t value, char *buffer)
{
    const char dec_chars[] = "0123456789";
    char *p = buffer; // Pointer to traverse the buffer
    char *p1 = p; // Pointer to mark the start of the number
    char tmp;

    if (value == 0)
    {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    while (value > 0)   /* Store digits in reverse order */
    {
        *p++ = dec_chars[value % 10];   /* Store the current digit */
        value /= 10;                   /* Remove the last digit */
    }
    *p = '\0';

    p--;
    while (p1 < p)  /* Reverse the string to get the correct order */
    {
        tmp = *p1;
        *p1++ = *p;
        *p-- = tmp;
    }
}

int main(void)
{
    uart_init(UBRRN);  /* Initialize UART with calculated UBRR value */
    init_adc();        /* Initialize ADC */

    char buffer_rv1[5];
    char buffer_ldr[5];
    char buffer_ntc[5];

    while (1)
    {
        uint16_t rv1_value = read_adc_channel(0);  /* Read ADC value from channel 0 (ADC0) */
        uint16_t ldr_value = read_adc_channel(1);  /* Read ADC value from channel 1 (ADC1) */
        uint16_t ntc_value = read_adc_channel(2);  /* Read ADC value from channel 2 (ADC2) */

        format_dec(rv1_value, buffer_rv1);
        format_dec(ldr_value, buffer_ldr);
        format_dec(ntc_value, buffer_ntc);

        uart_puts(buffer_rv1);                /* Transmit ADC value over UART */
        uart_puts(", ");
        uart_puts(buffer_ldr);                /* Transmit ADC value over UART */
        uart_puts(", ");
        uart_puts(buffer_ntc);                /* Transmit ADC value over UART */
        uart_puts("\r\n");                  /* New line for readability */
        _delay_ms(20);                  /* Delay for stability */
    }

    return 0;
}