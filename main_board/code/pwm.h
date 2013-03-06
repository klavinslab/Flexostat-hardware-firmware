/*
 * pwm.h
 *
 * Created: 8/2/2011 3:10:41 PM
 *  Author: soslab
 */ 
#ifndef PWM_H_
#define PWM_H_
#include <avr/io.h>


void pwm_init();
void pwm_set(uint8_t ch, uint16_t angle);
uint16_t millis();



#endif /* PWM_H_ */