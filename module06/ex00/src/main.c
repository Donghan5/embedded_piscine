#include "macro.h"

/*
** ---------------------------------------------------------------
** File: main.c
** Description: I2C protocol initialization and UART communication
** ressources: page 215 ~ (in datasheet)
** ---------------------------------------------------------------
*/


/*
**-------------------------------
** Tool Function
**-------------------------------
*/
char g_hex_str[5];
static char *byte_to_hex_str(uint8_t byte)
{
    char hex_digits[] = "0123456789ABCDEF";

    g_hex_str[0] = '0';
    g_hex_str[1] = 'x';

    g_hex_str[2] = hex_digits[(byte >> 4) & 0x0F];
    g_hex_str[3] = hex_digits[byte & 0x0F];

    g_hex_str[4] = '\0';

    return g_hex_str;
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
	uint8_t status;
	
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); /* Send START condition - Page 225 */

	while (!(TWCR & (1 << TWINT)));   /* Wait for TWINT flag set in TWCR Register (start condition is transmitted) - page 225 */

	status = (TWSR & 0xF8); /* Read TWI status register with masking the prescaler bits - page 226 */
    uart_puts("START condition sent. Status: ");
    uart_puts(byte_to_hex_str(status));
    uart_puts("\r\n");

	/* 
	** error check page 227 22-2 (a start condition has been transmitted)
	*/
	if (status != 0x08)
    {
        uart_puts(" -- ERROR: START or REPEATED START failed!\r\n");
    }

	/* send device address */
	TWDR = (0x38 << 1); 
	/* 
	** Write operation - page 228 22-2 2-wire Serial Bus will be released and not addressed
	** Slave mode entered
	** A START condition will be transmitted when the bus becomes free
	*/

	TWCR = (1<<TWINT) | (1<<TWEN); /* Clear TWINT to start transmission of address page 225 */
	while (!(TWCR & (1 << TWINT)));   /* Wait for TWINT flag set in TWCR Register (address is transmitted) - page 225 */
	
	status = (TWSR & 0xF8);
    uart_puts("SLA+W (0x70) sent. Status: ");
    uart_puts(byte_to_hex_str(status));
    uart_puts("\r\n");

	/*
	** Status check and error check.page 227 22-2
	** 0x18: SLA+W has been transmitted; ACK has been received
	** 0x20: SLA+W has been transmitted; NOT ACK has been received
	*/
    if (status == 0x18)
    {
        uart_puts(" -- OK: Slave ACK received.\r\n");
    }
    else if (status == 0x20)
    {
        uart_puts(" -- ERROR: Slave NACK received. (Device not found?)\r\n");
    }
    else
    {
        uart_puts(" -- ERROR: Unknown status after SLA+W.\r\n");
    }
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
	i2c_start();
	i2c_stop();

	while (1)
	{
		;
	}

	return 0;
}