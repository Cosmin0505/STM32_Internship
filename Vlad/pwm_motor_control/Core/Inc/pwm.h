/*
 * pwm.h
 *
 *  Created on: Aug 20, 2026
 *      Author: savav
 */

#ifndef INC_PWM_H_
#define INC_PWM_H_

#include "stm32f3xx.h"

void tim3_gpio_init(void);
void tim3_init();
void tim3_update_duty_cycle(uint32_t duty);

#endif /* INC_PWM_H_ */
