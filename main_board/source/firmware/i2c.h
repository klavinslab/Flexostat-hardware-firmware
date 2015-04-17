#ifndef I2C_H_
#define I2C_H_
#include <avr/io.h>

void i2c_master_init();
void master_send_i2c(uint8_t  , uint8_t  );


#endif /* I2C_H_ */