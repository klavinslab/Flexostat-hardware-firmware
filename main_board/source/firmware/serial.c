#include "serial.h"
#include "pwm.h"
#include "i2c.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/atomic.h>

#define DISABLE_TX_INT()  UCSR0B &= ~(1<<UDRIE0);
#define ENABLE_TX_INT()  UCSR0B |= (1<<UDRIE0);

#define BLOCKING_TX

void comm_init();

#define NUM_CMD 6
command cmdlist[NUM_CMD];

volatile uint8_t rxbuf[RX_BUFFER_SIZE];
volatile uint8_t txbuf[TX_BUFFER_SIZE];
volatile uint8_t rxsp, rxlen;  //start pointer, data length 0=>empty
volatile uint8_t txsp, txlen; //start point, data length
//start pointer is start of valid data
volatile int8_t cmdflag;
volatile uint8_t pulse_time[9]= {0,0,0,0,0,0,0,0,0}; // for SPV pulse mode.
const uint8_t pulse_time_len = (sizeof(pulse_time)/sizeof(pulse_time[0]));

//init serial 8N1
void usart_init() {
	comm_init();
	cmdflag = 0;
	rxsp = 0;
	rxlen = 0;
	txsp = 0;
	txlen = 0;
	//setup USART clock
	UBRR0 = UBRR_VAL;
	//UCSR0A = 0; //default value good.
	//enable TX and RX
	//enable RX complete interrupt and USART DR empty
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(1<<UDRIE0);
	//8 data bits
	UCSR0C = 0x06;

	DDRD |= 0x02; //setup TX to be an output

	//ENABLE_TX_INT(); //already done

	//for PV
	PORTA = 0; //close all PVs
	DDRA = 0xFF; //outputs
	DDRC = 0x80 | 0x40; // SPV outputs

}

uint8_t get_tx_len(){
	return txlen;
}

ISR(USART0_RX_vect) {
	uint8_t data = UDR0;
	if (rxlen < RX_BUFFER_SIZE) {
		rxbuf[(rxsp + rxlen)%RX_BUFFER_SIZE] = data;
		rxlen++;
	}
	if (data == COMMAND_CHAR){
		cmdflag++;
	}
}

ISR(USART0_UDRE_vect) {
	if (txlen>0) {
		UDR0 = txbuf[txsp];
		txsp = (txsp+1)%TX_BUFFER_SIZE;
		txlen--;
	} else {
		DISABLE_TX_INT();
	}
}

//returns 0 if buffer overflow; nothing printed
//returns 1 success!
uint8_t print_char (uint8_t c) {
	uint8_t retval = 0;
#ifndef BLOCKING_TX
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (txlen < TX_BUFFER_SIZE) {
			txbuf[(txsp + txlen)%TX_BUFFER_SIZE] = c;
			txlen++;
			ENABLE_TX_INT();
			retval = 1;
		}
	}
#else
	//Wait until the transmitter is ready

	while(!(UCSR0A & (1<<UDRE0)))
	{
		//Do nothing
	}

   //Now write the data to USART buffer

	UDR0=c;
	retval = 1;
#endif
	return retval;
}


//returns 0 if buffer overflow
//1 otherwise
uint8_t print_string (const uint8_t *s) {
	while (*s != 0) {
		if (print_char(*s)==0) {
			return 0;
		}
		s++;
	}
	return 1;
}

//returns 0 if buffer full
uint8_t print_ulong (uint32_t n) {
	uint8_t b[12];
	return print_string ((uint8_t*)ultoa(n,(char*)b,10));
}


//returns 0 if the buffer is empty
//only place in code that shrinks rxlen
uint8_t read_char () {
	uint8_t retval=0;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (rxlen>0) {
			retval = rxbuf[rxsp];
			rxlen--;
			rxsp = (rxsp+1)%RX_BUFFER_SIZE;
		}
	}
	return retval;
}


uint8_t* read_cmd(uint8_t* b) {
	uint8_t *rv = b;
	uint8_t temp;
	do {
		temp = read_char();
		*b = temp;
		b++;
	} while (temp != COMMAND_CHAR && temp != 0);
	--b;
	*b = 0; //null terminate.

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (temp == COMMAND_CHAR && cmdflag>0) { //>0 check just in case.
			cmdflag--;
		} else {
			cmdflag = 0;
		}
	}
	return rv;
}

void pv_on(uint8_t t) {
  if (t < 8) {
    PORTA |= 0x01<<t; //open SPV t
  } else {    
    switch (t)
    {
    case 8:
      PORTC |= 0x80; //PortC7
      break;
    case 9:
      PORTC |= 0x40; //PortC6
      break;
    }
  }
}

void pv_off(uint8_t t) {
  if (t < 8) {
    PORTA &= ~(0x01<<t); //open SPV t
  } else {    
    switch (t)
    {
    case 8:
      PORTC &= ~0x80; //PortC7
      break;
    case 9:
      PORTC &= ~0x40; //PortC6
      break;
    }
  } 
}


/*****COMMAND PARSING******/
uint8_t pulse(uint32_t t) {
  //command: pulnmmm; 
  //    n is chamber number,
  //    mmm WITH leading zeros is pulse duration max 255;
  uint8_t duration = (uint8_t)(t&0x000000FF);
  uint8_t valve = (uint8_t)((t&0x0000FFFF)/1000);
  
  //compensate for closing the valve at 1 instead of zero. 
  if (duration < 255) {
	duration += 1; 
  }
  if ( valve < pulse_time_len ) {
    pulse_time[valve]= duration;
  } 
  return 0;
}

uint8_t pv_select(uint32_t t) {
	//i2c pv
	if (t>0 && t<9) {
		master_send_i2c(114, (uint8_t) (t-1+'0'));
	}
	
	//SPV
	PORTC &= 0x3F; //0011 1111
	if (t < 8) { 
		PORTA = 0x01<<t; //open SPV t
	} else {  
		PORTA = 0; //close all SPV on port A
		switch (t)
		{
		case 8:
			PORTC |= 0x80; //PortC7
			break;
		case 9:
			PORTC |= 0x40; //PortC6
			break;
		}
	} 

	//servo
	if (t<5) {  //if mux PV 1
		pwm_set(2,0);
		pwm_set(1,(uint16_t)t*45*20);
		pwm_set(6,0);
		pwm_set(5,(uint16_t)t*45*20);
	} else if (t<=8){
		pwm_set(1,0);
		pwm_set(2,(uint16_t)(t-4)*45*20);
		pwm_set(5,0);
		pwm_set(6,(uint16_t)(t-4)*45*20);
	}
	return 0;
}
uint8_t pv_close_all(uint32_t t) {
	//pv_select(0);
	pwm_set(1,0);
	pwm_set(2,0);
	pwm_set(5,0);
	pwm_set(6,0);
	PORTA = 0x00; //close all SPVs
	PORTC &= 0x3F; //close port C SPVs
	return 0;
}
uint8_t pump_move_a(uint32_t t) {
	pwm_set(3,(uint16_t)t*2);
	return 0;
}
uint8_t pump_move_b(uint32_t t) {
	pwm_set(4,(uint16_t)t*2);
	return 0;
}

void comm_init() {
	/////Select PV command
	cmdlist[0].str[0] = 's';
	cmdlist[0].str[1] = 'e';
	cmdlist[0].str[2] = 'l';
	cmdlist[0].str[3] = 0;
	cmdlist[0].args = 1;
	cmdlist[0].fn = &pv_select;
	/////close PV command
	cmdlist[1].str[0] = 'c';
	cmdlist[1].str[1] = 'l';
	cmdlist[1].str[2] = 'o';
	cmdlist[1].str[3] = 0;
	cmdlist[1].args = 0;
	cmdlist[1].fn = &pv_close_all;
	/////move pump a (depricated; backward compatable with older firmware)
	cmdlist[2].str[0] = 'p';
	cmdlist[2].str[1] = 'm';
	cmdlist[2].str[2] = 'v';
	cmdlist[2].str[3] = 0;
	cmdlist[2].args = 1;
	cmdlist[2].fn = &pump_move_a;
	/////move pump a
	cmdlist[3].str[0] = 'p';
	cmdlist[3].str[1] = 'm';
	cmdlist[3].str[2] = 'a';
	cmdlist[3].str[3] = 0;
	cmdlist[3].args = 1;
	cmdlist[3].fn = &pump_move_a;
  /////move pump b
  cmdlist[4].str[0] = 'p';
  cmdlist[4].str[1] = 'm';
  cmdlist[4].str[2] = 'b';
  cmdlist[4].str[3] = 0;
  cmdlist[4].args = 1;
  cmdlist[4].fn = &pump_move_b;
  /////pulnmmm; pulse chamber n for mmm time 
  cmdlist[5].str[0] = 'p';
  cmdlist[5].str[1] = 'u';
  cmdlist[5].str[2] = 'l';
  cmdlist[5].str[3] = 0;
  cmdlist[5].args = 1;
  cmdlist[5].fn = &pulse;
}

void parse_command() {
	uint8_t match;
	uint32_t arg = 0;
	uint8_t p[TX_BUFFER_SIZE];
	read_cmd(p);
	uint8_t *c = p;
	eatspace(c);

	//for each command
	for (uint8_t ind = 0; ind <NUM_CMD; ind++) {
		match = 1;
		//check the first 3 chars for a string match
		for (uint8_t ind2 = 0; ind2 <3; ind2++) {
			if (cmdlist[ind].str[ind2] != c[ind2]) {
				match = 0;
				break;
			}
		}
		//on match, get input and call func
		if (match) {
			c = c+3;
			if(cmdlist[ind].args == 1) {
				eatspace(c);
				arg = strtoul((char*)c,NULL,10);
			}
			cmdlist[ind].fn(arg);
		}
	}

}
