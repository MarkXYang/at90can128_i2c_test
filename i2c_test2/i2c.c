#include "i2c.h"
#include <util/delay.h>

void initI2C(void) {
  TWBR = 32;                               /* set bit rate, see p. 242 */
                                     /* 8MHz / (16+2*TWBR*1) ~= 100kHz */
  TWDR = 0xFF;		// Default content = SDA released.
  TWCR |= (1 << TWEN)|                                 // Enable TWI-interface and release TWI pins.
	  (0<<TWIE)|(0<<TWINT)|                      // Disable Interrupt.
	  (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // No Signal requests.
	  (0<<TWWC);                                 //
}

void i2cWaitForComplete(void) {
	while(!(TWCR & (1<<TWINT))) {
	}
}

void i2cStart(void) {
  //TWCR = (_BV(TWINT) | _BV(TWEN) | _BV(TWSTA));
  TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
  i2cWaitForComplete();
}

void i2cStop(void) {
  //TWCR = (_BV(TWINT) | _BV(TWEN) | _BV(TWSTO));
  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

uint8_t i2cReadAck(void) {
  //TWCR = (_BV(TWINT) | _BV(TWEN) | _BV(TWEA));
  TWCR = ((1<<TWINT) | (1<<TWEN) | (1<<TWEA));
  i2cWaitForComplete();
  return (TWDR);
}

uint8_t i2cReadNoAck(void) {
  TWCR = ((1<<TWINT) | (1<<TWEN));
  i2cWaitForComplete();
  return (TWDR);
}

void i2cSend(uint8_t data) {
  TWDR = data;
  TWCR = ((1<<TWINT) | (1<<TWEN));		/* init and enable */
  i2cWaitForComplete();
}
