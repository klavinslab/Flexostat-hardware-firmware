/*
 * pwm.c
 *
 * Created: 8/2/2011 3:10:29 PM
 *  Author: soslab
 */ 
#include "pwm.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t epocht;

//setup stir motor PWM
//TODO: convert this to work with timer2
void pwm_init() {
	/******STIR INIT*******/
	DDRD |= 0x80 | 0x40;	
	
	//CTC mode, toggle on compare match A, B
	TCCR2A = 0x02 | 0x40 | 0x10;
	//cksel /1024, 
	TCCR2B = 0x07;
	
	//toggle on couter top and half counter top to create 90deg phase
	OCR2A = COUNTER_TOP;
	OCR2B = COUNTER_TOP/2;
	
	TIMSK2 = 1<<OCIE2A; 
	epocht = 0;
	
}

//one epoch is 2/STIR_SPEED seconds
ISR (TIMER2_COMPA_vect) {
	epocht++;	
}

//this is a low res estimate of current time
// it is roughly 2/STIR_SPEED of a second accurate.
// ie for STIR_SPEED=400 it is accurate to +/- 5 millis
uint32_t millis() {
	//note last set of () kills percision, but right now it divides evenly.
	return (epocht*(2000L/STIR_SPEED));
}

uint16_t epoch() {
	return epocht;
}