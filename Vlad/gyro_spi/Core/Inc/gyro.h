/*
 * gyro.h
 *
 *  Created on: Aug 10, 2026
 *      Author: savav
 */

#ifndef INC_GYRO_H_
#define INC_GYRO_H_

#include <stdint.h>

#define CS_SET (1U << 3)
#define CS_RESET (1U << (3 + 16))

#define L3GD20_READ 0x80u
#define L3GD20_WRITE 0x00u
#define L3GD20_AUTOINC 0x40u
#define L3GD20_WHO_AM_I 0x0Fu
#define L3GD20_CTRL_REG1 0x20u
#define L3GD20_CTRL_REG4 0x23u
#define L3GD20_CTRL_REG5 0x24u
#define L3GD20_STATUS_REG 0x27u
#define L3GD20_OUT_X_L 0x28u
#define L3GD20_STATUS_ZYXDA (1U << 3)

#define CAL_DISCARD_SAMPLES    20u   /* let the 12.5 Hz LPF settle           */
#define CAL_SAMPLE_COUNT      128u   /* 128 / 95 Hz ~= 1.35 s of holding still */

/* Stationary noise at 12.5 Hz BW is roughly 12 LSB RMS, so ~70 LSB peak-to-
 * peak over 128 samples. 300 LSB (~2.6 dps p-p) tolerates that comfortably
 * while still catching a bumped desk. Loosen it if you get false rejects. */
#define CAL_MAX_SPREAD_LSB     300

/* Datasheet ZRL spec is +/-10 dps = ~1143 LSB. A mean beyond ~1400 LSB means
 * the board was rotating steadily, or the part is out of spec.             */
#define CAL_MAX_BIAS_LSB      1400

typedef struct {
    int16_t x, y, z;
    uint8_t valid;
} gyro_bias_t;

void spi1_gpio_init(void);
void spi1_init(void);
uint8_t spi1_transfer(uint8_t tx);
uint8_t gyro_read_reg(uint8_t reg);
void gyro_write_reg(uint8_t reg, uint8_t value);
void gyro_init(void);
void gyro_read_xyz(int16_t *x, int16_t *y, int16_t *z);
int32_t raw_to_mdps(int32_t raw);
void gyro_wait_data_ready(void);
void gyro_read_xyz_mdps(volatile int32_t *x, volatile int32_t *y, volatile int32_t *z);
uint8_t gyro_calibrate(void);

#endif /* INC_GYRO_H_ */
