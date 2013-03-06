/*
 * od.c
 *
 * Created: 8/2/2011 10:47:29 AM
 *  Author: chris
 */ 
#include <avr/interrupt.h>
#include "od.h"
#include "pwm.h"

void od_init_board_specific();

void od_init() {
	od_init_board_specific();
	
	//LF channel select
	//rest of the pins are unconnected so we can change the whole register
	DDRB |= 0x07;
	PORTB = 0x00; //for now select ONLY the first port	
}


void od_init_board_specific() {
 //setup serial
 
	//setup USART clock
	UBRR1 = UBRR_VAL1;
	//UCSR0A = 0; //default value good. 
	//enable TX and RX
	//no interrupts here, all IO will be blocking
	UCSR1B = (1<<RXEN1)|(1<<TXEN1); 
	//8 data bits
	UCSR1C = 0x06;
	
	DDRD |= 0x08; //TX is an output
 	
}	

#define READ_TIMEOUT 500
rawOD od_get() {
	uint8_t byte_num;
	volatile uint8_t foo;
	rawOD rod;
	rod.values[0] = 5;
	rod.values[1] = 5;
	
	int16_t time0 = millis(); //for timeout
	while (!(UCSR1A&(1<<UDRE1 ))) {
		; //wait for TX empty
	}
	//flush input buffer
	while (UCSR1A & (1<<RXC1)) {
		foo = UDR1;  
	}
	UDR1 = ';'; //send something
	//now wait to get 8 bytes back
	byte_num = 0;

	while(byte_num<sizeof(rawOD) && (millis()-time0)<READ_TIMEOUT ) {
		while( !(UCSR1A & (1<<RXC1)) && (millis()-time0)<READ_TIMEOUT )
			; 
		rod.bytes[byte_num] = UDR1;
		byte_num++;
	}

	
	if(!((millis()-time0)<READ_TIMEOUT)) {
		rod.values[0] = 0;// TX==0 means something is wrong!
	}
	
	return rod;
}	

