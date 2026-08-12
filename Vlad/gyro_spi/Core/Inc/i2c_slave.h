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
#define GYRO_PACKET_LEN 16u

#define GYRO_ST_CAL_VALID     (1u << 0)
#define GYRO_ST_SATURATED     (1u << 1)
#define GYRO_ST_SENSOR_FAULT  (1u << 2)

void i2c_slave_init(void);
void i2c_slave_publish(int32_t x, int32_t y, int32_t z, uint8_t status);
void i2c_slave_get_packet(uint8_t *dst);
void dump_packet_hex(void);

#endif /* INC_I2C_SLAVE_H_ */
