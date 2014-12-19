/*
 * LFreader.c
 *
 * Created: 10/4/2011 4:37:14 PM
 *  Author: soslab
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "pwm.h"
#include "serial.h"
#include "od.h"

#define INTEGRATION_TIME 2000  //2 sec
#define OPEN_PV() PORTB |= (0x01<<1)
#define CLOSE_PV() PORTB &= ~(0x01<<1)
//good to about 1/2 second
void pause(uint16_t duration) {
  uint8_t d = duration/5;
  uint8_t ts = epoch();
  uint8_t t = ts;
  while (((int8_t)(t-ts)) < d) {
    t = epoch();
  }
}

int main(void)
{ 
  uint32_t rx_val=0;
  uint32_t tx_val=0;
  uint16_t cur_time;
  uint16_t close_time=0;
  DDRB = 0x01<<1;
  PORTD = 0x01<<6; // pull D6 high to enable stirring.
  pwm_init();
  usart_init();
  od_init();
  sei();
  od_get_rx();
  od_get_tx();
  uint16_t lastepoch = epoch();
  
  while(1)
  {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      cur_time = millis();
      if ((cur_time-lastepoch) >= INTEGRATION_TIME) {  
        rx_val = od_get_rx();
        tx_val = od_get_tx();
        lastepoch = cur_time;
      }
    }   
    
    if (get_rx_len()>0) {
      rawOD r;
      uint8_t cnt = 0;
      uint8_t cmd_char = read_char();
      r.values[0] = tx_val;
      r.values[1] = rx_val;
      
      while (cnt<sizeof(rawOD)) {
        print_char(r.bytes[cnt]);
        cnt++;
      }
      if (cmd_char>0) {
        close_time = cur_time + (((uint16_t)cmd_char)*5);
        OPEN_PV();
      }
    }
    if ((close_time-cur_time)&0x8000){ //if negative.
      CLOSE_PV();
    }
  }
}