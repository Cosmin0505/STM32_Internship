/*
 * i2c.c
 *
 *  Created on: Aug 18, 2026
 *      Author: savav
 */
#include "i2c.h"

/* Configure PB8 as SCL and PB9 as SDA for I2C1 */
void i2c1_gpio_init(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	(void)RCC->AHBENR;

	GPIOB->MODER &= ~((3U << (8 * 2)) | (3U << (9 * 2)));
	GPIOB->MODER |= ((2U << (8 * 2)) | (2U << (9 * 2)));

	GPIOB->OTYPER |= (1U << 8) | (1U << 9);

	GPIOB->OSPEEDR |= (3U << (8 * 2)) | (3U << (9 * 2));

	GPIOB->PUPDR &= ~((3U << (8 * 2)) | (3U << (9 * 2)));

	/* Select AF4 for both pins */
	GPIOB->AFR[1] &= ~((0xFU << (8 - 8) * 4) | (0xFU << (9 - 8) * 4));
	GPIOB->AFR[1] |= (4U << (8 - 8) * 4) | (4U << (9 - 8) * 4);
}

/* Configure I2C1 in master mode @ 100kHz */
void i2c1_master_init(void)
{
	i2c1_gpio_init();

	/* Select the 8 MHz HSI clock as I2C1 clock source */
	RCC->CFGR3 &= ~RCC_CFGR3_I2C1SW;

	/* Enable I2C1 clock */
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	(void)RCC->APB1ENR; // Wait for initialization to complete

	/* Disable I2C1 before configuration */
	I2C1->CR1 &= ~I2C_CR1_PE;
	while(I2C1->CR1 & I2C_CR1_PE){ }

	/* 100khz @ fI2Cclk 8MHz */
	I2C1->TIMINGR = 0x10420F13U;
	I2C1->CR2 = 0U;

	/* Clear pending status flags */
	I2C1->ICR |= I2C_ICR_STOPCF
			   | I2C_ICR_NACKCF
			   | I2C_ICR_BERRCF
			   | I2C_ICR_ARLOCF
			   | I2C_ICR_OVRCF;

	/* Enable interrupt sources */
	I2C1->CR1 |= I2C_CR1_TXIE
			   | I2C_CR1_STOPIE
			   | I2C_CR1_NACKIE
			   | I2C_CR1_ERRIE;

	/* Enable I2C1 */
	I2C1->CR1 |= I2C_CR1_PE;

}

/* Write a buffer to a slave using its 7-bit address */
void i2c1_write(uint8_t addr, uint8_t *data, uint32_t length)
{
	/* Wait if I2C bus is busy */
	while(I2C1->ISR & I2C_ISR_BUSY)
	{

	}

	/* Clear previous transfer status flags */
	I2C1->ICR = I2C_ICR_STOPCF |
			    I2C_ICR_NACKCF |
				I2C_ICR_BERRCF |
				I2C_ICR_ARLOCF |
				I2C_ICR_OVRCF;

	/* Set the address, byte count and automatic stop signal and start transfer */
	I2C1->CR2 = ((uint32_t) addr << 1U) |
				(length << 16U) |
				I2C_CR2_AUTOEND |
				I2C_CR2_START;

	/* Send each byte from buffer when transmit register is available */
	for(int i = 0; i < length; i++)
	{
		while(!(I2C1->ISR & I2C_ISR_TXIS))
		{
		}
		I2C1->TXDR = data[i];
	}

	/* Wait until automatic stop signal is transmitted */
	while(!(I2C1->ISR & I2C_ISR_STOPF))
	{
	}

	I2C1->ICR = I2C_ICR_STOPCF;
}
