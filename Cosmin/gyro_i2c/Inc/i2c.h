/*
 * i2c.h
 *
 *  Created on: Aug 10, 2026
 *      Author: busilam
 */

#ifndef I2C_H_
#define I2C_H_

#define I2C1_OWN_ADDRESS		(0x50)
#define I2C_BYTE_TO_SEND		(0xA5)
#define BTN_PIN					(1U << 13)
#define	LED_PIN					(1U << 5)

void gpio_led_init(void);
void gpio_button_init(void);
void exti_init(void);
void i2c2_init(void);
void i2c1_init(void);
void gpio_i2c2_init_master(void);
void gpio_i2c1_init_slave(void);


#endif /* I2C_H_ */
