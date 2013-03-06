/*
 * od.h
 *
 * Created: 8/2/2011 10:47:46 AM
 *  Author: chris
 */ 


#ifndef OD_H_
#define OD_H_

typedef union __rodd 
{
	uint8_t bytes[2*sizeof(uint32_t)];
	uint32_t values[2];
} rawOD;

void od_init();

rawOD od_get();

#define od_select(p) 	PORTB = (p&0x07)

//FOR USART
#define FOSC 16000000L
//#define BAUD_RATE 57600
#define BAUD_RATE1 19200
#define UBRR_VAL1 ((uint16_t)(FOSC/16/BAUD_RATE1 - 1))




#endif /* OD_H_ */