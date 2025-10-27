#include <avr/io.h>
#include <util/delay.h>


/*
**  Debouncing: Noise filttering
**  Noise come from chattering (bounced multiple time)
*/
int main(void)
{
    DDRB |= (1 << DDB0);    // Pin mode setting (like init the variable)

    while (1)
    {
        if (!(PIND & (1 << PIND2)))
        {
            _delay_ms(20);   // to avoid bound effect

            if (!(PIND & (1 << PIND2))) // Checking buttons pressed after 20ms
            {
                PORTB ^= (1 << PORTB0); // Used XOR operator (effective way to turn on(off) the machine)
                
                while (!(PIND & (1 << PIND2)))  // When the button is pushed, do nothing
                {
                    ;
                }
            }
        }
    }
}