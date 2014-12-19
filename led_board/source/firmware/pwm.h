/*
 * pwm.h
 *
 * Created: 8/2/2011 3:10:41 PM
 *  Author: soslab
 */ 
#ifndef PWM_H_
#define PWM_H_
#include <avr/io.h>

extern uint8_t tmr2ofcnt;
uint16_t millis();
uint16_t epoch();
void pwm_init();

/////STIR/////
//NOTE: changing STIRS_SPEED so it is not a factor of 2000
// will break millis().  ie 2000L/STIR_SPEED should have no remainder.
#define STIR_SPEED 400 //edges per second

//this is defined by the prescalar so don't change this!
//unit is edges per second (i.e. steps per second)
//prescalar is 1/1024.  2 edges per timer overflow, 8mhz
//timer max is 2^8
//note: >>18 is /(1024*2^8)
#define MIN_STIR ((2*8000000L)>>18)  //61 edges per second ~ 1.5 rev/sec
//desired edges per second
#define COUNTER_TOP (MIN_STIR*(1L<<8)/STIR_SPEED - 1)

#endif /* PWM_H_ */
