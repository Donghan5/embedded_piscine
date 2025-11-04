#include "macro.h"

/*
**-------------------------------
** UART Function
**-------------------------------
*/
void uart_init(unsigned int ubrr)
{
    /* Set baud rate */
    UBRR0H = (unsigned char)(ubrr >> 8);    /* Upper Rate part */
    UBRR0L = (unsigned char)(ubrr);         /* Lower Rate part */

    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);  /* Enable Receiver and Transmitter */
    
    UCSR0C = (3 << UCSZ00);  /* Format to 8N1 (N - No parity, and Data is 8 bits)*/
    UCSR0A |= (1 << U2X0); /* Enabling double transfer speed, and divide by two baudrate (Reduce Error Rate) */
}

void uart_tx(char c)
{
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

/*
**-------------------------------
** ADC Function
**-------------------------------
*/
void init_adc(void)
{
    ADMUX = (1 << REFS0);

    ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); /* Enable ADC and set prescaler to 128 (16MHz/128 = 125KHz) (page 259 24-5) */
}

uint16_t adc_read(void)    /*PAGE 256*/
{
    ADCSRA |= (1 << ADSC);               /* Start single conversion */

    while (ADCSRA & (1 << ADSC))       /* Wait for conversion to complete */
    {
        ;
    }
    
    return ADC; /* Return ADC value 10bit resolution (not have to convert "((uint16_t)high << 8) | low") */
}

/*
**-------------------------------
** RGB Function
**-------------------------------
*/
void init_rgb()
{
    SET_RGB_OUTPUTS(); /* Set RGB pins as output */

    TCCR0A |= (1 << COM0A1) | (1 << COM0B1); /* page 113 15-3 / page 114 15-6  Setting pwm mode (Non-inverting PWM) */
    TCCR0A |= (1 << WGM01) | (1 << WGM00);  /* page 115 - 15-8 mode 3 top 255 (counter 0 to 255 (rgb value)) */
    TCCR0B |= (1 << CS00);  /* Do not prescaling (to avoid Flicker) page 116 */

    TCCR2A |= (1 << COM2B1); /* page 163 18-6 set OC2B at bottom*/
    TCCR2A |= (1 << WGM21) | (1 << WGM20);  /* page 164 18-8 model 3 */
    TCCR2B |= (1 << CS20); /* Do not prescaling page 165 */

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
** 10 bit ADC value (0 ~ 1023)
** Light up LEDs based on ADC value (d1 ~ d4)
*/
void led_gauge(uint16_t adc_value)
{
    PORT_OFF(); /* Turn off all LEDs */

    if (adc_value >= 256)
        PORTB |= (1 << LED_D1);
    if (adc_value >= 512)
        PORTB |= (1 << LED_D2);
    if (adc_value >= 768)
        PORTB |= (1 << LED_D3);
    if (adc_value >= 1010) // 100% (All LEDs ON)
        PORTB |= (1 << LED_D4);
}

void wheel(uint8_t pos) 
{
    pos = 255 - pos;
    
    if (pos < 85) {
        set_rgb(255 - pos * 3, 0, pos * 3);
    } else if (pos < 170) {
        pos = pos - 85;
        set_rgb(0, pos * 3, 255 - pos * 3);
    } else {
        pos = pos - 170;
        set_rgb(pos * 3, 255 - pos * 3, 0);
    }
}

int main(void)
{
    uart_init(UBRRN);  /* Initialize UART with calculated UBRR value */
    init_adc();        /* Initialize ADC */
    init_rgb();        /* Initialize RGB PWM */
    PORT_ON();      /* Turn on all LEDs */

    while (1)
    {
        uint16_t adc_value = adc_read();
        led_gauge(adc_value);
        wheel(adc_value >> 2); /* Convert 10bit to 8bit by right shifting 2 bits */
        _delay_ms(20);
    }

    return 0;
}