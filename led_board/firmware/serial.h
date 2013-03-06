/*
 * serial.h
 *
 * Created: 8/1/2011 9:14:10 PM
 *  Author: chris
 */ 
#ifndef SERIAL_H_
#define SERIAL_H_
#include <avr/io.h>

//FOR USART
#define FOSC 8000000L
//#define BAUD_RATE 57600
#define BAUD_RATE 19200
#define UBRR_VAL ((uint16_t)(FOSC/16/BAUD_RATE - 1))
#define RX_BUFFER_SIZE 20
#define TX_BUFFER_SIZE 80
#define COMMAND_CHAR ';'


/*******************FOR COMMANDS***************/
///////////MACROFUNCTIONS
#define isnum(x) ( (x>='0')&&(x<='9') )
#define eatspace(x) while(*x == ' '){x++;}
	
///////////types
typedef struct {
	uint8_t str[4];  //command string
	uint8_t args;  //number of args, 0 or 1
	uint8_t (*fn)(uint32_t);
} command;

/***************PROTOTYPES**********************/
void usart_init();

uint8_t print_char (uint8_t c);
uint8_t print_string (const uint8_t *s);
uint8_t print_ulong (uint32_t n);
uint8_t get_rx_len ();
uint8_t read_char ();
uint8_t* read_string(uint8_t* b);

uint8_t get_tx_len();

void parse_command();

extern volatile uint8_t cmdflag;


#endif /* SERIAL_H_ */