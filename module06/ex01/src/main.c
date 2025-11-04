#include "macro.h"

/*
** ---------------------------------------------------------------
** File: main.c
** Description: I2C protocol initialization and UART communication
** ressources: page 215 ~ (in datasheet)
** ---------------------------------------------------------------
*/


/* Function prototype */
void uart_puts(char *str);
void i2c_write(unsigned char data);
void i2c_stop(void);

/*
**-------------------------------
** Tool Function
**-------------------------------
*/
char g_hex_str[5];
void print_hex_value(char c)
{
    char hex_digits[] = "0123456789ABCDEF";

    g_hex_str[0] = hex_digits[(c >> 4) & 0x0F];
	g_hex_str[1] = hex_digits[c & 0x0F];

    g_hex_str[2] = '\0';
	uart_puts(g_hex_str);
}

/*
**-------------------------------
** UART Initialization Function
**-------------------------------
*/
void uart_init(unsigned int ubrr)
{
    /* Set baud rate */
    UBRR0H = (unsigned char)(ubrr >> 8);    /* Upper Rate part */
    UBRR0L = (unsigned char)(ubrr);         /* Lower Rate part */

    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);  /* Enable Receiver and Transmitter */
    
    UCSR0C = (3 << UCSZ00);  /* Format to 8N1 (N - No parity, and Data is 8 bits)*/
    UCSR0A |= (1 << U2X0); /* Enabling double transfer speed, and divide by two baudrate (Reduce Error Rate) */
}

void uart_tx(char c)
{
    while (!(UCSR0A & (1 << UDRE0)))   /* Wait for empty transmit buffer */
    {
        ;
    }

    /* UDR0 - USART Data Register 0  (REAL BOX which put the data)*/
    UDR0 = c;                           /* Put data into buffer, sends the data */
}

void uart_puts(char *str)
{
    while (*str)
    {
        uart_tx(*str++);
    }
}

/*
**-------------------------------
** I2C Function
**-------------------------------
*/
void i2c_init(void)
{
	/*
	** TWSR - TWI Status Register 
	** TWBR - TWI Bit Rate Register (SCL frequency setting) - I2C standard Clock Speed -> 100kHz
	** TWCR - TWI Control Register
	*/
	TWSR = 0x00;             /* Prescaler value = 1 */
	/*
	** Additional Note:
	** SCL frequency = CPU clock / (16 + 2 * TWBR * prescaler)
	** TWBR = ((F_CPU / SCL) - 16) / (2 * prescaler)
	*/
	TWBR = ((F_CPU / 100000) - 16) / 2; /* Set bit rate for 100kHz */
	TWCR = (1 << TWEN);     /* Enable TWI - page 240 (description) */
}

void i2c_start(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); /* Send START condition - Page 225 */

	while (!(TWCR & (1 << TWINT)));   /* Wait for TWINT flag set in TWCR Register (start condition is transmitted) - page 225 */
}

/*
** Acknowledge read (with ACK)
** Send 7-bit address + Read bit
*/
uint8_t i2c_read_ack(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); /* Enable TWI, generation of ACK after reception */

	while (!(TWCR & (1 << TWINT)));   /* Wait for TWINT flag set in TWCR Register (data is received) - page 225 */

	return TWDR; /* Return received data */
}

/*
** Non-Acknowledge read (with NACK)
** Send 7-bit address + Read bit
** known as completion of data reception
*/
uint8_t i2c_read_nack(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN); /* Enable TWI, generation of NACK after reception */

	while (!(TWCR & (1 << TWINT)));   /* Wait for TWINT flag set in TWCR Register (data is received) - page 225 */

	return TWDR; /* Return received data */
}

void i2c_read(void)
{
	uint8_t data[7];

	i2c_start();

	i2c_write(I2C_ADDRESS_AHT20 | 0x01); /* SLA+R */

	for (uint8_t i = 0; i < 6; i++)
	{
		data[i] = i2c_read_ack();
	}
	data[6] = i2c_read_nack();
	i2c_stop();

	for (uint8_t i = 0; i < 7; i++)
	{
		print_hex_value(data[i]);
		uart_puts(" ");
	}
}

void i2c_write(unsigned char data)
{
	TWDR = data; /* Load data into TWDR Register */
	TWCR = (1 << TWINT) | (1 << TWEN); /* Start transmission of data */
	while (!(TWCR & (1 << TWINT)));   /* Wait for TWINT flag set in TWCR Register (data is transmitted) - page 225 */
}

void i2c_stop(void)
{
	/* Stop condition - page 225 */
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

int main(void)
{
	uart_init(UBRRN);
	i2c_init();

	while (1)
	{
		i2c_start();

		i2c_write(I2C_ADDRESS_AHT20);
		i2c_write(MEASUREMENT_CMD);
		i2c_write(0x33);
		i2c_write(0x00);

		i2c_stop();

		_delay_ms(1000);
		i2c_read();
		uart_puts("\r\n");
		_delay_ms(1000);
	}

	return 0;
}