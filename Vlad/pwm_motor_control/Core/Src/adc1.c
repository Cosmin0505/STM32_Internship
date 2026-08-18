/*
 * adc1.c
 *
 *  Created on: Aug 18, 2026
 *      Author: savav
 */
#include "adc1.h"

void gpio_init(void)
{
	RCC->AHBENR |= GPIOAEN;
	GPIOA->MODER &= ~(3U << (0 * 2));
	GPIOA->MODER |= (3U << (0 * 2));
}

void adc1_init(void)
{
	gpio_init();

	RCC->AHBENR |= RCC_AHBENR_ADC12EN;

    ADC1_2_COMMON->CCR &= ~(3U << 16);
    ADC1_2_COMMON->CCR |= (2U << 16);


	ADC1->CR &= ~(ADC_CR_ADVREGEN_1 | ADC_CR_ADVREGEN_0);
	ADC1->CR |= ADC_CR_ADVREGEN_0;

	for(int i = 0; i < 1000; i++)
	{

	}

	ADC1->SQR1 |= 1U << 6;


	ADC1->CR |= ADC_CR_ADEN;

	while(!(ADC1->ISR & ADC_ISR_ADRDY))
	{

	}
}

void adc1_start_conversion(void)
{
	ADC1->CR |= ADC_CR_ADSTART;
}

uint32_t adc1_read(void)
{
	while(!(ADC1->ISR & ADC_ISR_EOS))
	{

	}

	return (uint32_t)ADC1->DR;
}

