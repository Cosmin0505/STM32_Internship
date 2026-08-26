/*
 * adc1.h
 *
 *  Created on: Aug 18, 2026
 *      Author: savav
 */

#ifndef INC_ADC1_H_
#define INC_ADC1_H_

#include "stm32f3xx.h"

#define GPIOAEN (1U << 17);

void adc1_init(void);
void adc1_start_conversion(void);
uint32_t adc1_read(void);

#endif /* INC_ADC1_H_ */
