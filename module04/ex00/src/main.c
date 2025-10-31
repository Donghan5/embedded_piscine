#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define SET_MODE(x, num) DDR##x |= (1 << DD##x##num)
#define PORT_TOGGLE(x, num) PORT##x ^= (1 << PORT##x##num)

volatile int g_flag = 0;

/*
** External Interrupts - Condition based interrupts
** INT0_vect - External Interrupt Request 0
*/
ISR(INT0_vect)
{
    if (g_flag == 0)
    {
        g_flag = 1;

    }
}

void enable_interrupts(void)
{
    
    EIMSK |= (1 << INT0); /* Enable INT0 page 81 */

}

int main(void)
{
    SET_MODE(B, 0); /* Set PB0 as output */

    enable_interrupts();

    sei(); /* Enable global interrupts */

    while (1)
    {
        if (g_flag == 1)
        {
            PORT_TOGGLE(B, 0); /* Toggle PB0 */
            
            _delay_ms(42);

            g_flag = 0;
        }
    }
    
}