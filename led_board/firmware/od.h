/*
 * od.h
 *
 * Created: 8/2/2011 10:47:46 AM
 *  Author: chris
 */ 


#ifndef OD_H_
#define OD_H_

void od_init();
uint32_t od_get_rx();
uint32_t od_get_tx();

typedef union __rodd 
{
	uint8_t bytes[2*sizeof(uint32_t)];
	uint32_t values[2];
} rawOD;

#define EN_T0_INT() TIMSK0 = 0x01
#define DIS_T0_INT() TIMSK0 = 0x00

#define EN_T1_INT() TIMSK1 = 0x01
#define DIS_T1_INT() TIMSK1 = 0x00 


#endif /* OD_H_ */