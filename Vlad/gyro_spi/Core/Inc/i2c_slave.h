/*
 * i2c_slave.h
 *
 *  Created on: Aug 12, 2026
 *      Author: savav
 */

#ifndef INC_I2C_SLAVE_H_
#define INC_I2C_SLAVE_H_

#include <stdint.h>

#define I2C_SLAVE_ADDR_7BIT 0x42u
#define GYRO_PACKET_LEN 12u

void i2c_slave_init(void);
void i2c_slave_publish(int32_t x, int32_t y, int32_t z);

#endif /* INC_I2C_SLAVE_H_ */
