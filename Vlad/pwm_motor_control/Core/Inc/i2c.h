/*
 * i2c.h
 *
 *  Created on: Aug 18, 2026
 *      Author: savav
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_

#include "stm32f3xx.h"
#include <stdint.h>



void i2c1_master_init(void);
void i2c1_write(uint8_t addr, uint8_t *data, uint32_t length);

#endif /* INC_I2C_H_ */
