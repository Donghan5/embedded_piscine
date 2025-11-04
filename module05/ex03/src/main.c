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
    ** REFS1 - Reference Selection Bit 1 -> Internal 1.1V Voltage Reference (page 257 24-3) Voltage Reference Selection
    **       - set in INTERNAL
    ** ADLAR - ADC Left Adjust Result (page 257) (for 8 bit resolution we need left adjust result) -< if 0, 10 bit resolution
    ** RESF0 - INPUT / ADLAR - OUTPUT
    */
    ADMUX = (1 << REFS0) | (1 << REFS1) | (1 << MUX3); // AVcc with internal 1.1V Voltage Reference at AREF pin (1000 - Internal Temperature Sensor) 

    /*
    ** ADCSRA - ADC Control and Status Register A
    ** ADEN - ADC Enable
    ** ADPS2:0 - ADC Prescaler Select Bits (page 259 24-5)
    */
    ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); /* Enable ADC and set prescaler to 128 (16MHz/128 = 125KHz) (page 259 24-5) */
}

uint16_t read_adc_celsius(void)    /*PAGE 256*/
{
    ADCSRA |= (1 << ADSC);               /* Start single conversion */

    while (ADCSRA & (1 << ADSC))       /* Wait for conversion to complete */
    {
        ;
    }
    
    return ADC; /* Return ADC value 10bit resolution (not have to convert "((uint16_t)high << 8) | low") */
}

/*
** 1.08 --> 1100(mv)/1024(adc max) = 1.074(mv per adc)
** So we need to multiply adc value with 1.08 to get mv
** Then we need to divide it by 10 to get celsius
*/
uint16_t convert_to_celsius(uint16_t adc_value)
{
    int32_t temp_calc = (int32_t)adc_value * 108;

    temp_calc = temp_calc / 100;

    return (int16_t)temp_calc - 273; /* minus Kelvin offset */
}

/*
** Format climate
** Just following the instructions (not precise temperature conversion)
*/
void format_climate(uint8_t value, char *buffer)
{
    if (value < 0)
    {
        *buffer++ = '-';
        value = -value;
    }

    buffer[0] = (value / 10) + '0';       /* Tens place */
    buffer[1] = (value % 10) + '0';       /* Units place */
    buffer[2] = '\0';                     /* Null-terminate the string */
}

int main(void)
{
    uart_init(UBRRN);  /* Initialize UART with calculated UBRR value */
    init_adc();        /* Initialize ADC */

    char buffer[8];

    while (1)
    {
        uint16_t adc_value = read_adc_celsius();  /* Read ADC value from internal temperature sensor (ADC0) */
        uint16_t celsius = convert_to_celsius(adc_value); /* Convert ADC value to Celsius temperature */
        format_climate(celsius, buffer);
        uart_puts(buffer);
        uart_puts("\r\n");
        _delay_ms(20);                  /* Delay for stability */
    }

    return 0;
}