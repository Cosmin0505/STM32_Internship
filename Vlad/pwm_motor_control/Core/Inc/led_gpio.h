/*
 * led_gpio.h
 *
 *  Created on: Aug 18, 2026
 *      Author: savav
 */

#ifndef INC_LED_GPIO_H_
#define INC_LED_GPIO_H_

#include "stm32f3xx.h"

#define GPIODEN (1U << 20)

void gpiod_init(void);
void green_led(uint8_t input);
void yellow_led(uint8_t input);
void red_led(uint8_t input);

#endif /* INC_LED_GPIO_H_ */
