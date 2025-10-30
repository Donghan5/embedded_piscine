#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

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
    init_rgb();

    while (1)
    {
        for (uint8_t i = 0; i < 255; i++) {
            wheel(i);
            _delay_ms(42);
        }
    }
}