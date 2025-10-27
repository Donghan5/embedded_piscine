#include <avr/io.h>

int main(void)
{
    /*
    ** Pin mode setting. (Port B 0 pin set to output)
    ** DDRB: Data Direction Register for Port B
    ** DDB0: Data Direction B port 0
    */
    DDRB |= (1 << DDB0);


    /*
    ** Change pin status
    ** PORTB (PORT B Data Register)
    */
    PORTB |= (1 << PORTB0);

    while (1)
    {
        // infinite loop
    }

    return 0;
}