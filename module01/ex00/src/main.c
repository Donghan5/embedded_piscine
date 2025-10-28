#include <avr/io.h>

/*
** Calculation
** Defined f cpu 16 000 000
** Object -> to make 1ms
*/
void ft_delay_ms(uint16_t ms)
{
    // Calculation inner loop --> to calculate 1ms
    // uint32_t -> 32bit, AVR -> 4bit (predict 4~8 cycle)
    const uint32_t inner_loop_count = (F_CPU / 1000) / 40;

    for (uint16_t i = 0; i < ms; i++)
    {
        volatile uint32_t j;    // it prevent compiler optimization, if we don't use this keyword -> inner loop will be deleted
        for (j = 0; j < inner_loop_count; j++)
        {
            ;
        }
    }
}


int main(void)
{
    DDRB = (1 << DDB1);

    while (1)
    {
        ft_delay_ms(500);
        PORTB ^= (1 << PORTB1);
    }
}