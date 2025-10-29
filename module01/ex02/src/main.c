#include <avr/io.h>


#define SET_MODE(x, num) DDR##x |= (1 << DD##x##num) /* Setting the output mode */


int main(void)
{
    SET_MODE(B, 1);

    TCCR1A |= (1 << COM1A1);    /* Compare Output Mode 1 A, bit 1 */
    TCCR1B |= ((1 << CS12));    /* Define Scale (256) */

    /* 
    ** Fast mode setting set each wgm to 1 (Creation of Wave, NOT CTC mode)
    ** WGM11 -> TCCR1A (dataset)
    ** WGM12/WGM13 -> TCCR1B (dataset)
    */
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM13) | (1 << WGM12);
    

    /*
    ** Counting Sequence
    **  - Single-slope operation -> BOTTOM to TOP
    **  - TOP is defined by ICR1 (Input Register Capture)
    **  - BOTTOM -> OCR1 (Output Compare Register)
    **  - When operation arrive at BOTTOM, update
    ** OCR1A - Output Compare Register 1A -> setting the limit
    **  - Here I will define 10% of ICR1
    */
    ICR1 = 62499;   /* F_CPU(16000000) / prescaler(256) - 1 (counter) */
    OCR1A = 6250;

    while (1)
    {
        ;
    }
}