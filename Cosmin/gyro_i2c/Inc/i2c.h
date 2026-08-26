/*
 * i2c.h
 *
 *  Created on: Aug 10, 2026
 *      Author: busilam
 */

#ifndef I2C_H_
#define I2C_H_

/* Address of Discovery */
#define DISCOVERY_I2C_ADDR_7BIT		(0x42U)
/* Length of buffer of received values */
#define GYRO_XYZ_LEN				12U

/* User button pin */
#define BTN_PIN						(1U << 13)
/* Led pin */
#define	LED_PIN						(1U << 5)

/* Initialize GPIO for onboard led */
void gpio_led_init(void);
/* Initialize GPIO for user button */
void gpio_button_init(void);
/* Initialize EXTI */
void exti_init(void);
/* Initialize I2C2 */
void i2c2_init(void);
/* Initialize GPIO for I2C2 */
void gpio_i2c2_init_master(void);


#endif /* I2C_H_ */
