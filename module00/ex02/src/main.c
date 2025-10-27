#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    /*
    ** Pin mode setting. (Port B 0 pin set to output)
    ** DDRB: Data Direction Register for Port B
    ** DDB0: Data Direction B port 0
    */
    DDRB |= (1 << DDB0);

    /*
    ** PIND - The Port D Input Pins Address
    */
    while (1)
    {
        if (!(PIND & (1 << PIND2)))
        {
            PORTB |= (1 << PORTB0); // B port 0 -> HIGH(5V) --> LED ON 
        }
        else
        {
            PORTB &= ~(1 << PORTB0);    // B port 0 --> LOW(0V) --> LED OFF
        }
    }

}