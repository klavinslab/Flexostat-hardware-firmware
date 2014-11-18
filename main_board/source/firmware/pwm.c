/*
 * pwm.c
 *
 * Created: 8/2/2011 3:10:29 PM
 *  Author: soslab
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "pwm.h"
#include "serial.h"

volatile uint16_t time;

void pwm_init() {
	/*****PWM TIMER0******/
	DDRB |= 0x18;  //set OC0A and OC0B to outputs
	DDRD |= 0x80 | 0x40; // set OC2A and OC2B to outputs
	//clear on compare match, set on BOTTOM, FAST PWM
	TCCR0A = 1<<COM0A1 | 1<<COM0B1 | 1<<WGM01 | 1<<WGM00;
	TCCR0B = 0x05; //clock div 1024
	//clear on compare match, set on BOTTOM, FAST PWM
	TCCR2A = 1<<COM0A1 | 1<<COM0B1 | 1<<WGM01 | 1<<WGM00;
	TCCR2B = 0x04 | 0x02 | 0x01; //clock div 1024
	//NOTES: Period ~= 16ms instead of 20ms... we'll see how that works
	//1 "count" in OCR reg is 64uS
	//10uS = 1degree
	//so, "center" is 23.4=23 counts
	//90 deg is 9.4=9 and 37.5=37
	//45deg is 16.4, 30.4
	OCR0A = 23; //center
	OCR0B = 23; //center
	OCR2A = 23;
	OCR2B = 23;
	
	
	/*****PWM TIMER1******/ 
	//this is for pwm channels 1 2 3
	//set as outputs
	DDRD |= 0x30;
	TCCR1A = 0xAA; // non inverting on all three channels, WGM1:0 = 0b10
	TCCR1B = 0x1A; // /8 prescalar, WGM3:2 = 0b11
	//roll over every 40000*8 cycles = 20ms period
	//this also implies that one count is 0.5uS
	ICR1 = 40000-1; 
	//set all PWM outputs to dead center
	OCR1A = 3000;//1500uS
	OCR1B = 3000;//1500uS
	TIMSK1 = 1<<TOV1; //turn on "overflow" interrupt for RTC
	
	/*****PWM TIMER2******/
	DDRD |= 0xC0;  //set OC0A and OC0B to ouputs
	//clear on compare match, set on BOTTOM, FAST PWM
	TCCR2A = 1<<COM2A1 | 1<<COM2B1 | 1<<WGM21 | 1<<WGM20;
	TCCR0B = 0x05; //clock div 1024
	//NOTES: Period ~= 16ms instead of 20ms... we'll see how that works
	//1 "count" in OCR reg is 64uS = 6.4degrees
	//10uS = 1degree
	//so, "center" is 23.4=23 counts
	//90 deg is 9.4=9 and 37.5=37
	//45deg is 16.4, 30.4
	OCR0A = 23; //center
	OCR0B = 23; //center
	
}


ISR (TIMER1_OVF_vect) {
	time = time+20; //interrupts happen every 20ms
  uint8_t ch=0;
  while (ch<pulse_time_len) {
    if (pulse_time[ch]>1) {  //if count is big keep valve open and continue on
      pulse_time[ch]--;
      pv_on(ch);
    } else if (pulse_time[ch] == 1){ //close on 1 count and do nothing on 0.
	  pulse_time[ch]--;
      pv_off(ch);
    }
	ch++;
  }
}

uint16_t millis() {
	//could make this more accurate by adding the timer value to time
	return time;
}

//set angle in units of 1/20th of a degree
//0_degrees is defined as all the way CCW (when facing the gear)
void pwm_set(uint8_t ch, uint16_t angle) {
	angle = angle + 600*2;
	if (angle > (600*2 + 180*20)) {
		angle = (600*2 + 180*20);
	}
	switch (ch) {
		case 1:
			OCR0A = (uint8_t)(angle/(2*64)); //angle/20/6.4
			break;
		case 2:
			OCR0B = (uint8_t)(angle/(2*64)); //angle/20/6.4
			break;
		case 3:
			OCR1A = angle;
			break;
		case 4:
			OCR1B = angle;
			break;
		case 5:
			OCR2A = (uint8_t)(angle/(2*64)); //angle/20/6.4
			break;
		case 6:
			OCR2B = (uint8_t)(angle/(2*64)); //angle/20/6.4
			break;
	}
	
}