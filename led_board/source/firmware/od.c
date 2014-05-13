/*
 * od.c
 *
 * Created: 8/2/2011 10:47:29 AM
 *  Author: chris
 */ 
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "od.h"

//overflow counters
volatile uint16_t rxofcnt;
volatile uint16_t txofcnt;

//TODO: REWORK for newtarget not that millis now needs to use 
// timer2 so it must be moved to pwm.c
void od_init() { 
	rxofcnt = 0;  //connected to T1
	txofcnt = 0;  //connected to T0
	
	//clock T0 on rising edge of LF input
	TCCR0B |= 1<<CS00 | 1<<CS01 | 1<<CS02;
	//enable timer0 overflow interrupt
	EN_T0_INT();
	
	//clock T1 on rising edge of LF input
	TCCR1B |= 1<<CS10 | 1<<CS11 | 1<<CS12;
	//enable timer1 overflow interrupt
	EN_T1_INT();
	
	
}

ISR (TIMER0_OVF_vect) {
	txofcnt++;	
}

ISR (TIMER1_OVF_vect) {
	rxofcnt++;
}

uint32_t od_get_tx() { //T0
	static uint32_t last = 0;
	uint8_t tcnt0_val;
	uint32_t current;
	uint32_t retval;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		tcnt0_val = TCNT0;
		//if we overflowed but the interrupt wasn't handled yet
		if ( (tcnt0_val<2) && (TIFR0&(1<<TOV0)) ) { 
			txofcnt++;
			TIFR0 = 1<<TOV0; //clear the interrupt
		}
		current = (((uint32_t)txofcnt)<<8)|((uint32_t)tcnt0_val);
	}	
	retval = current - last;
	//clear the top byte because we're really only doing 24bit math here
	retval &= 0x00FFFFFFl;
	
	last = current;
	return retval;
}

uint32_t od_get_rx() { //T1
	static uint32_t last = 0;
	uint16_t tcnt1_val;
	uint32_t current;
	uint32_t retval;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		tcnt1_val = TCNT1;
		//if we overflowed but the interrupt wasn't handled yet
		if ( (tcnt1_val<2) && (TIFR1&(1<<TOV1)) ) { 
			rxofcnt++;
			TIFR1 = 1<<TOV1; //clear the interrupt
		}
		current = (((uint32_t)rxofcnt)<<16)|((uint32_t)tcnt1_val);
	}
	retval = current - last;
	
	last = current;
	return retval;
}
