#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void set_rgb(uint8_t red, uint8_t green, uint8_t blue)
{
    /* Pre Setting about all colors 0b1 == 0001  set to 1 */
    red &= 0b1;
    green &= 0b1;
    blue &= 0b1;

    PORTD = (red << PORTD5) | (green << PORTD6) | (blue << PORTD3);    /* Setting port on */
    _delay_ms(1000);    /* Wait 1 sec */
}

int main(void)
{
    DDRD |= (1 << DDD3) | (1 << DDD5) | (1 << DDD6);    /* Register setting */

    while (1)
    {
        set_rgb(0xff, 0, 0);    /* red */
        set_rgb(0, 0xff, 0);    /* green */
        set_rgb(0, 0, 0xff);    /* blue */
        set_rgb(0xff, 0xff, 0); /* yellow */
        set_rgb(0xff, 0, 0xff); /* magenta */
        set_rgb(0, 0xff, 0xff); /* cyan */
        set_rgb(0xff, 0xff, 0xff);  /*white*/
    }
}