#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define SET_MODE_OUTPUT_MULTIPLE() DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4)
#define SET_MODE_PIN_READ(x, num) PIN##x & (1 << P##x##num)
#define CLEAR_PORTB() (PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4)))
#define CLEAR_DDRX(x) (DDR##x &= ~((1 << PD2) | (1 << PD4)))
#define SET_SW_INPUT(x) PORT##x |= (1 << P##x##2) | (1 << P##x##4)

/* Store previous switch states */
volatile uint8_t g_prev_sw1_state;
volatile uint8_t g_prev_sw2_state;

/* Store LED state */
volatile uint8_t g_led_state = 0;

ISR(TIMER0_COMPA_vect)
{
    uint8_t current_sw1_state = SET_MODE_PIN_READ(D, 2);
    uint8_t current_sw2_state = SET_MODE_PIN_READ(D, 4);

    if (current_sw1_state == 0 && g_prev_sw1_state != 0)
    {
        if (g_led_state < 15)   /* limited to 15 (avoid overflow) */
        {
            g_led_state++;
        }
        _delay_ms(150); /* Simple debouncing */
    }

    if (current_sw2_state == 0 && g_prev_sw2_state != 0)
    {
        if (g_led_state > 0)   /* limited to 0 (avoid underflow) */
        {
            g_led_state--;
        }
        _delay_ms(150); /* Simple debouncing */
    }

    /* Save current values as previous values */
    g_prev_sw1_state = current_sw1_state;
    g_prev_sw2_state = current_sw2_state;

    /*
    ** Prepare LED output value
    ** 3 LSBs for 3 LEDs, 4th bit for another LED (to pb0, pb1, pb2)
    */
    uint8_t led_output = g_led_state & 0x07; // Limit to 3 bits

    if (g_led_state & 0x08) /* if 4th bit (the '8's place, 0x08) is set */
    {
        led_output |= (1 << PB4);   /* Set PB4 */
    }

    CLEAR_PORTB();
    PORTB |= led_output;

    _delay_ms(10);

}


int main(void)
{
    SET_MODE_OUTPUT_MULTIPLE();

    CLEAR_PORTB();  /* Clear resource for sure */

    CLEAR_DDRX(D);  /* Clear DDRD for sure */

    SET_SW_INPUT(D);

    g_prev_sw1_state = SET_MODE_PIN_READ(D, 2);
    g_prev_sw2_state = SET_MODE_PIN_READ(D, 4);

    /* Timer0 configuration */
    TCCR0A = (1 << WGM01); /* CTC mode page 115 15-8 */

    TCCR0B = (1 << CS02) | (1 << CS00); /* Prescaler 1024 page 117 15-9 */

    /* Set compare match value */
    OCR0A = 156; /* 10ms at 16MHz with 1024 prescaler */

    TIMSK0 = (1 << OCIE0A); /* Enable compare match interrupt */

    sei(); /* Enable global interrupts */

    while (1)
    {

    }
}