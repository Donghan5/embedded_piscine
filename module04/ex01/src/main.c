#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define SET_PB1_AS_OUTPUT() (DDRB |= (1 << PB1))

volatile uint8_t g_duty_percent = 0;    /* 0 - 100% duty cycle */
volatile int8_t g_step = 2;    /* Timer0 interrupt -> 100HZ */

ISR(TIMER0_COMPA_vect)  /* Timer0 Compare A Match interrupt (send the PWM signal) */
{
    g_duty_percent += g_step;

    if (g_duty_percent >= 100)
    {
        g_duty_percent = 100;
        g_step = -2;    /* Change direction */
    }

    if (g_duty_percent == 0)
    {
        g_step = 2;    /* Change direction */
    }

    /* 
    ** Update OCR1A register
    ** 8-bit resolution: 0 - 255
    ** duty_percent: 0 - 100
    ** Scaling formula: (duty_percent * 255) / 100
    */
    OCR1A = ((uint16_t)g_duty_percent * 255) / 100;

}

int main(void)
{
    // OC1A (PB1) as output
    SET_PB1_AS_OUTPUT();

    /* Timer1 initialization */
    TCCR1A = (1 << COM1A1) | (1 << WGM10);    /* Non-inverting mode, 8-bit PWM page 140 (part of mode 5) */
    TCCR1B = (1 << WGM12) | (1 << CS11);    /* PWM mode, prescaler = 8 page 142 (part of mode 5) */


    /* Timer0 initialization */
    TCCR0A = (1 << WGM01);    /* CTC mode page 115 15-8 */
    OCR0A = 155;    /* 16MHz CPU / 1024 prescaler / 156 (OCR0A+1) = 99.8Hz (approx 100Hz) */

    TIMSK0 = (1 << OCIE0A);    /* Enable Timer0 Compare A Match interrupt page 118 */

    /*
    ** We set frequency of Timer0 to 100Hz
    ** So the duty cycle will be updated every 10ms
    ** if we use 1024 prescaler:
    ** f_timer0 = f_cpu / prescaler = 16,000,000 / 1024 ~= 16000Hz
    ** To get 100Hz, we need to set OCR0A to 155
    */
    TCCR0B = (1 << CS02) | (1 << CS00); /* page 117 15-9 1024 scaler */

    sei();    /* Enable global interrupts */

    while (1) {}

}