/*
 * LFreader.c
 *
 * Created: 10/4/2011 4:37:14 PM
 *  Author: soslab
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "pwm.h"
#include "serial.h"
#include "od.h"

#define INTEGRATION_TIME (2*STIR_SPEED/2)  //2 sec

//good to about 1/2 second
void pause(uint16_t duration) {
	uint8_t d = duration/5;
	uint8_t ts = epoch();
	uint8_t t = ts;
	while (((int8_t)(t-ts)) < d) {
		t = epoch();
	}
}

int main(void)
{	
	uint32_t rx_val;
	uint32_t tx_val;
	uint16_t curtime;
	DDRB = 0x00;
	pwm_init();
	usart_init();
	od_init();
	sei();
	od_get_rx();
	od_get_tx();
	uint16_t lastepoch = epoch();
	
	while(1)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			curtime = epoch();
			if ((curtime-lastepoch) >= INTEGRATION_TIME) {	
				rx_val = od_get_rx();
				tx_val = od_get_tx();
				lastepoch = curtime;
			}
		}		
		
		if (get_rx_len()>0) {
			rawOD r;
			uint8_t cnt = 0;
			read_char();
			r.values[0] = tx_val;
			r.values[1] = rx_val;
			
			while (cnt<sizeof(rawOD)) {
				print_char(r.bytes[cnt]);
				cnt++;
			}
		}

	}
}