/*
 * pwm.c
 *
 *  Created on: Aug 20, 2026
 *      Author: savav
 */
#include "pwm.h"

/* Configure pin PA6 */
void tim3_gpio_init(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	GPIOA->MODER &= ~(3U << (6 * 2));
	GPIOA->MODER |= (2U << (6 * 2));

	/* Select AF2 (TIM3_CH1) for PA6 */
	GPIOA->AFR[0] &= ~(0xFU << (6 * 4));
	GPIOA->AFR[0] |= (2U << (6 * 4));
}

/* TIM3 frequency is 48MHz */
void tim3_init()
{
	tim3_gpio_init();

	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	/* Timer tick frequency is ftim3 / (TIM3_PSC + 1) => 48 / 48 MHz = 1 MHz */
	TIM3->PSC = 47;
	/* PWM frequency is ftim3 / ((PSC + 1) * (ARR + 1)) */
	TIM3->ARR = 999;

	/* Select PWM mode for TIM3_CH1 */
	TIM3->CCMR1 &= ~(0xFU << 4);
	TIM3->CCMR1 |= (6U << 4);

	/* Enable pre-load to ensure safe duty cycle updates */
	TIM3->CCMR1 |= TIM_CCMR1_OC1PE;

	/* Enable CH1 output and TIM3 peripheral */
	TIM3->CCER |= TIM_CCER_CC1E;
	TIM3->CR1 |= TIM_CR1_CEN;
}

void tim3_update_duty_cycle(uint32_t duty)
{
	TIM3->CCR1 = duty;
}

