/*
 * sys_clock.c
 *
 *  Created on: Jul 30, 2026
 *      Author: savav
 */

#include "sys_clock.h"

void clock_init(void)
{
	FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_1;

	RCC->CR |= RCC_CR_HSION;
	while(!(RCC->CR & RCC_CR_HSIRDY));

	RCC->CR &= ~RCC_CR_PLLON;
	while(RCC->CR & RCC_CR_PLLRDY);

	RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMUL |
					RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
	RCC->CFGR |= (RCC_CFGR_PLLMUL12
				| RCC_CFGR_HPRE_DIV1
				| RCC_CFGR_PPRE1_DIV2
				| RCC_CFGR_PPRE2_DIV1);

	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));

	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

	RCC->CFGR3 &= ~RCC_CFGR3_USART1SW;

}
