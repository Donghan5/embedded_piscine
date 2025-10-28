#include <avr/io.h>
#include <util/delay.h>

/*
** DDB3: Special functions
**  - MOSI: SPI transfer (Master Out Slave In)
**  - OC2A: Timer and counter (precise controller)
*/
void update_leds(unsigned char count)
{
    PORTB = 0;  // Vide canvas

    // Re-mapping
    if (count & 0b1000)  // if 4th bit is 1? (the value is over than 8)
    {
        PORTB |= (1 << PORTB4); // turn on the 4th bit
        count ^= 0b1000; // 4th bit count 0
    }
    PORTB |= count; // Basic part, 0th 2nd bit send to 0th 2nd pin
}

int main(void)
{
    unsigned char count = 0;

    DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB4);   // DDB3 assigned for
    while (1)
    {
        _delay_ms(20);   // to avoid bound effect
        if (!(PIND & (1 << PIND2)) && count < 15)
        {
            count++;
            update_leds(count);

            while (!(PIND & (1 << PIND2)))
            {
                ;
            }
        }
        else if (!(PIND & (1 << PIND4)) && count > 0)
        {
            count--;
            update_leds(count);

            while (!(PIND & (1 << PIND4)))
            {
                ;
            }
        }
    }
}