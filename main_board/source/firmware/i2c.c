
#include "i2c.h"
#include <avr/interrupt.h>

uint8_t tw_send_dat;
uint8_t tw_send_addr;

//assumes 16mhz
//assumes 50khz i2c
void i2c_master_init() {
	//pull up SDA SCL
	PORTC |= 0x01; 
	PORTC |= 0x02; 
	
	TWBR = 76;  //50khz  scales linearly...
	TWSR |= 0x01; // TW PS = 4^1=4
	
	//clear any interrupt flag as we'd have no idea what it's for at this point
	TWCR |= (1<<TWINT);
}

//only good for sending 1 byte
void master_send_i2c(uint8_t addr, uint8_t data) {
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN) | (1<<TWIE);
	tw_send_addr = addr<<1;  //set address+W
	tw_send_dat = data;
	//now wait for start sent interrupt.
}


ISR(TWI_vect) {
	switch (TWSR & 0xf8) {
		case 0x10: //REPEAT START
		case 0x08: //START sent successfully
			TWDR = tw_send_addr;
			TWCR &= ~((1<<TWSTA)|(1<<TWSTO));  //no start no stop (probably don't need to clear these)
			break;
		case 0x18: //SLA+W has been transmitted
			TWDR = tw_send_dat; //load send data
			TWCR &= ~((1<<TWSTA)|(1<<TWSTO));  //no start no stop (probably don't need to clear these)
			break;
		case 0x20: //transmitted addr and got NACK
		case 0x30: //transmitted data and got NACK
			//just give up
		case 0x28: //transmitted data and got ACK
			TWCR &= ~(1<<TWSTA);  //no start
			TWCR |= (1<<TWSTO);  //stop
			break;
		case 0x38: //arbitration lost
			;//not dealing with this because we're a single master
	}
	
	//clear interrupt
	TWCR |= (1<<TWINT);
}