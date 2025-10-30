#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define PORT_CHECK(x, num) PORT##x & (1 << PORT##x##num)    /* Verification of the port is turned-on */
#define PORT_ON(x, num) PORT##x = (1 << PORT##x##num)   /* Allocation of the port (different with |= )*/

void next_color(void)
{
    if (PORT_CHECK(D, 5))
    {
        PORT_ON(D, 6);
    }
    else if (PORT_CHECK(D, 6))
    {
        PORT_ON(D, 3);
    }
    else if (PORT_CHECK(D, 3))
    {
        PORT_ON(D, 5);
    }
    else
    {
        PORT_ON(D, 5);
    }
}

int main()
{
    /*
    ** D5 - red
    ** D6 - green
    ** D3 - blue
    */
    DDRD |= (1 << DDD5) | (1 << DDD6) | (1 << DDD3);    /* Register to write */

    while (1)
    {
        next_color();
        _delay_ms(1000);
    }
}