/*
 * led_gpio.c
 *
 *  Created on: Aug 18, 2026
 *      Author: savav
 */

#include "led_gpio.h"

void gpiod_init(void)
{
	RCC->AHBENR |= GPIODEN;

	/* PD9, PD11, PD13 output mode */
	GPIOD->MODER &= ~((3U << (9 * 2)) | (3U << (11 * 2)) | (3U << (13 * 2)));
	GPIOD->MODER |= ((1U << (9 * 2)) | (1U << (11 * 2)) | (1U << (13 * 2)));

	GPIOD->OSPEEDR &= ~((3U << (9 * 2)) | (3U << (11 * 2)) | (3U << (13 * 2)));
	GPIOD->OSPEEDR |= ((3U << (9 * 2)) | (3U << (11 * 2)) | (3U << (13 * 2)));

	GPIOD->OTYPER &= ~((1U << 9) | (1U << 11) | (1U << 13));

	GPIOD->PUPDR &= ~(3U << (9 * 2) | (3U << (11 * 2)) | (3U << (13 * 2)));
}

void green_led(uint8_t input)
{
	if(input == 0)
	{
		GPIOD->ODR &= ~(1 << 9);
	}
	else
	{
		GPIOD->ODR |= (1 << 9);
	}
}

void yellow_led(uint8_t input)
{
	if(input == 0)
	{
		GPIOD->ODR &= ~(1 << 11);
	}
	else
	{
		GPIOD->ODR |= (1 << 11);
	}
}

void red_led(uint8_t input)
{
	if(input == 0)
	{
		GPIOD->ODR &= ~(1 << 13);
	}
	else
	{
		GPIOD->ODR |= (1 << 13);
	}
}
