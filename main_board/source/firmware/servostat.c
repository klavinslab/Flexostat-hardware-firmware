/*
 * servostat.c
 *
 * Created: 8/1/2011 8:31:22 PM
 *  Author: chris
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "serial.h"
#include "od.h"
#include "pwm.h"

//uint8_t chambers[] = {4,0,1,5,2,6,3,7};
uint8_t chambers[] = {0,1,2,3,4,5,6,7};

int main(void)
{
	uint8_t chamber_num = 0;
	uint16_t st;
	uint32_t odtx,odrx;  //some temps
	rawOD rod; //raw OD reading
	
	usart_init();
	od_init();
	pwm_init();
	sei();
	
	st = millis();
	od_select(chambers[chamber_num]);
  while(1)
  {
		//deal with commands while waiting
		do {
			while (cmdflag) {
				parse_command();
			}
		} while (millis()-st < 3000);	
		st = millis();  //set this here so the period is exactly 1000mS.
		
		chamber_num = 0;
		
		while (chamber_num <sizeof(chambers)){	
			od_select(chambers[chamber_num]);			
			chamber_num++;
			
			rod = od_get();
			odtx = rod.values[0];
			odrx = rod.values[1];
		
			//print OD data
			print_ulong(odtx);
			print_char(' ');
			print_ulong(odrx);
			print_char(' ');
		}		
			
		print_char('\r');
		print_char('\n');


  }
}

