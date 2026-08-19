/*
 * i2c.c
 *
 *  Created on: Aug 18, 2026
 *      Author: savav
 */
#include "i2c.h"

void i2c1_gpio_init(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	(void)RCC->AHBENR;

	/* Configure pins PB8 and PB9 */
	GPIOB->MODER &= ~((3U << (8 * 2)) | (3U << (9 * 2)));
	GPIOB->MODER |= ((2U << (8 * 2)) | (2U << (9 * 2)));

	GPIOB->OTYPER |= (1U << 8) | (1U << 9);

	GPIOB->OSPEEDR |= (3U << (8 * 2)) | (3U << (9 * 2));

	GPIOB->PUPDR &= ~((3U << (8 * 2)) | (3U << (9 * 2)));

	GPIOB->AFR[1] &= ~((0xFU << (8 - 8) * 4) | (0xFU << (9 - 8) * 4));
	GPIOB->AFR[1] |= (4U << (8 - 8) * 4) | (4U << (9 - 8) * 4);
}

void i2c1_master_init(void)
{
	i2c1_gpio_init();

	RCC->CFGR3 &= ~RCC_CFGR3_I2C1SW;

	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	(void)RCC->APB1ENR; // wait for initialization complete

	I2C1->CR1 &= ~I2C_CR1_PE;
	while(I2C1->CR1 & I2C_CR1_PE){ }

	/* 100khz @ fI2Cclk 8MHz */
	I2C1->TIMINGR = 0x10420F13U;
	I2C1->CR2 = 0U;

	I2C1->ICR |= I2C_ICR_STOPCF
			   | I2C_ICR_NACKCF
			   | I2C_ICR_BERRCF
			   | I2C_ICR_ARLOCF
			   | I2C_ICR_OVRCF;
	I2C1->CR1 |= I2C_CR1_TXIE
			   | I2C_CR1_STOPIE
			   | I2C_CR1_NACKIE
			   | I2C_CR1_ERRIE;

	I2C1->CR1 |= I2C_CR1_PE;

}

void i2c1_write(uint8_t addr, uint8_t *data, uint32_t length)
{
	while(I2C1->ISR & I2C_ISR_BUSY)
	{

	}

	I2C1->ICR = I2C_ICR_STOPCF |
			    I2C_ICR_NACKCF |
				I2C_ICR_BERRCF |
				I2C_ICR_ARLOCF |
				I2C_ICR_OVRCF;

	I2C1->CR2 = ((uint32_t) addr << 1U) |
				(length << 16U) |
				I2C_CR2_AUTOEND |
				I2C_CR2_START;

	for(int i = 0; i < length; i++)
	{
		while(!(I2C1->ISR & I2C_ISR_TXIS))
		{
		}
		I2C1->TXDR = data[i];
	}

	while(!(I2C1->ISR & I2C_ISR_STOPF))
	{
	}

	I2C1->ICR = I2C_ICR_STOPCF;
}
