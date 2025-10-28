#include <avr/io.h>

int main(void)
{
    // DDB1 as an output - settings
    DDRB |= (1 << DDB1);
    
    /*
    ** TCCR - timer register
    **  - Timer/Counter0: 8bit (TCCR0A, TCCR0B ...)
    **  - Timer/Counter1: 16bit (TCCR1A, TCCR1B ...)
    **  - Timer/Counter2: 8bit, support async (TCCR2A, TCCR2B ...)
    ** WGM12 - CTC mode (Clear Timer on Compare Match)
    */
    TCCR1B |= (1 << WGM12);

    /*
    ** COM1A0 - Compare Output Mode 1 A, bit 0. Define the action when Compare match (OC1A / PORTB)
    */
    TCCR1A |= (1 << COM1A0);

    /*
    ** CS12 - Clock Select 1, bit 2 - declare speed (Prescaler)
    */
    TCCR1B |= ((1 << CS12));

    /*
    ** OCR1A - Output Compare Register 1A -> set the limit
    ** 31249 -> (F_CPU / (FREQUENCY * PRESCALE)) - 1
    */
    OCR1A = 31249;

    while (1)
    {
        ;
    }

}