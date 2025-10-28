#include <avr/io.h>
#include <util/delay.h>

#define PIN_PRESS(num) (PIND & (1 << num)) /* Define SW1 and SW2 pressed PID2 == 2(bit) // PIND4 == 4(bit) */
#define SET_MODE(x, num) DDR##x |= (1 << DD##x##num)  /* Define output */
#define MAX_COUNT 10    /* Define max duty recycle (100%) */
#define MIN_COUNT 1     /* Define min duty recycle (10%) */
#define TEN_PERC 6250   /* 10% of TOP value (ICR1) */

void anti_bounce(int num)
{
    while (!PIN_PRESS(num))
    {
        ;
    }
    _delay_ms(20);
}


void is_pressed(int num, uint8_t *counter)
{
    if (num == 2 && (*counter) < MAX_COUNT) 
    {
        if (!PIN_PRESS(2)) 
        {
            (*counter)++;
            OCR1A = (*counter) * TEN_PERC;
            anti_bounce(2);
        }
    }
    else if (num == 4 && (*counter) > MIN_COUNT)
    {
        if (!PIN_PRESS(4))
        {
            (*counter)--;
            OCR1A = (*counter) * TEN_PERC;
            anti_bounce(4);
        }
    }
    else
    {
        return;
    }
    
}


int main(void)
{
    SET_MODE(B, 1);

    TCCR1A |= (1 << COM1A1);    /* Define output */
    TCCR1B |= ((1 << CS12));    /* Define prescaler as 256 (cycle) */
    
    /* Fast mode setting */
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM13) | (1 << WGM12);

    uint8_t counter = MIN_COUNT;
    
    // TOP value
    ICR1 = 62499;   /* F_CPU(16000000) / prescaler(256) - 1 (counter) */
    
    OCR1A = (counter) * TEN_PERC;
    
    while (1)
    {
        is_pressed(2, &counter);
        is_pressed(4, &counter);
    }
}