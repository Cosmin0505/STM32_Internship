/*
 * i2c.h
 *
 *  Created on: Aug 10, 2026
 *      Author: busilam
 */

#ifndef I2C_H_
#define I2C_H_

#define DISCOVERY_I2C_ADDR_7BIT		(0x42U)
#define I2C_TEST_BYTE				(0xA5U)
#define BTN_PIN					(1U << 13)
#define	LED_PIN					(1U << 5)

void gpio_led_init(void);
void gpio_button_init(void);
void exti_init(void);
void i2c2_init(void);
void gpio_i2c2_init_master(void);


#endif /* I2C_H_ */
