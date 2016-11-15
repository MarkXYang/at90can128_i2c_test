/*
 * i2c_test2.c
 *
 * Created: 2016-11-11 3:06:28 PM
 * Author : brlxyang
 */ 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include "i2c.h"
#include "uart_gcc_drv.h"

// -------- Defines -------- //

#define LM75_ADDRESS_W               0b10010000
#define LM75_ADDRESS_R               0b10010001
#define LM75_TEMP_REGISTER           0b00000000
#define LM75_CONFIG_REGISTER         0b00000001
#define LM75_THYST_REGISTER          0b00000010
#define LM75_TOS_REGISTER            0b00000011
// -------- Functions --------- //

FILE uart_str = FDEV_SETUP_STREAM(uart_put_char, uart_get_char, _FDEV_SETUP_RW);

int main(void) {

	uint8_t tempHighByte, tempLowByte;

	// -------- Inits --------- //
	clock_prescale_set(clock_div_1);                             /* 8MHz */
	uart_init();
	
	stdout = stdin = stderr = &uart_str;
	
	printf("\r\n====  i2c Thermometer  ====\r\n");
	initI2C();
	sei();

	while (1) {
		/* To set register, address LM75 in write mode */
		i2cStart();
		i2cSend(LM75_ADDRESS_W);
		i2cSend(LM75_TEMP_REGISTER);
		i2cStart();                      /* restart, just send start again */
		/* Setup and send address, with read bit */
		i2cSend(LM75_ADDRESS_R);
		/* Now receive two bytes of temperature */
		tempHighByte = i2cReadAck();
		tempLowByte = i2cReadNoAck();
		i2cStop();

		// Print it out nicely over serial for now...
		printf("%d", tempHighByte);
		if (tempLowByte & _BV(7)) {
			printf(".5\r\n");
		}
		else {
			printf(".0\r\n");
		}
		/* Once per second */
		_delay_ms(5000);

	}                                                  /* End event loop */
	return 0;                            /* This line is never reached */
}
