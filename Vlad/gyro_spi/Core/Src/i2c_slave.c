/*
 * i2c_slave.c
 *
 *  Created on: Aug 12, 2026
 *      Author: savav
 */

#include "i2c_slave.h"
#include "usart1.h"
#include "stm32f3xx.h"
#include <string.h>
#include <stdio.h>

static uint8_t g_staging[GYRO_PACKET_LEN];
static uint8_t g_tx_buf[GYRO_PACKET_LEN];
static volatile uint8_t g_tx_index;

volatile uint32_t g_i2c_berr, g_i2c_arlo, g_i2c_ovr;

static void put_le32(uint8_t *dst, int32_t v)
{
	uint32_t u = (uint32_t) v;

	dst[0] = (uint8_t)(u & 0xFFu);
	dst[1] = (uint8_t)((u >> 8) & 0xFFu);
	dst[2] = (uint8_t)((u >> 16) & 0xFFu);
	dst[3] = (uint8_t)((u >> 24) & 0xFFu);
}

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

void i2c_slave_init(void)
{
	i2c1_gpio_init();

	RCC->CFGR3 &= ~RCC_CFGR3_I2C1SW;

	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	(void)RCC->APB1ENR; // wait for initialization complete

	I2C1->CR1 &= ~I2C_CR1_PE;
	while(I2C1->CR1 & I2C_CR1_PE){ }

	/* 100khz @ fI2Cclk 8MHz */
	I2C1->TIMINGR = 0x10420F13U;

	I2C1->OAR1 = 0U;
	I2C1->OAR1 = I2C_OAR1_OA1EN | ((uint32_t) I2C_SLAVE_ADDR_7BIT << 1);
	I2C1->OAR2 = 0U;
	I2C1->CR2 = 0U;

	I2C1->CR1 &= ~I2C_CR1_NOSTRETCH;

	I2C1->CR1 |= I2C_CR1_ADDRIE
			   | I2C_CR1_RXIE
			   | I2C_CR1_TXIE
			   | I2C_CR1_STOPIE
			   | I2C_CR1_NACKIE
			   | I2C_CR1_ERRIE;

	I2C1->CR1 |= I2C_CR1_PE;

	NVIC_SetPriority(I2C1_EV_IRQn, 1U);
	NVIC_EnableIRQ(I2C1_EV_IRQn);
	NVIC_SetPriority(I2C1_ER_IRQn, 1U);
	NVIC_EnableIRQ(I2C1_ER_IRQn);
}

void i2c_slave_publish(int32_t x, int32_t y, int32_t z)
{

	// Check if interrupts were enabled before
	uint32_t primask = __get_PRIMASK();
	__disable_irq();

	/* Convert x/y/z values into little-endian and store them inside staging buffer  */
	put_le32(&g_staging[0], x);
	put_le32(&g_staging[4], y);
	put_le32(&g_staging[8], z);

	__DMB();

	//If interrupts were enabled before, re-enable them
	if(primask == 0U)
	{
		__enable_irq();
	}
}

static void i2c_take_snapshot(void)
{
	memcpy(g_tx_buf, g_staging, GYRO_PACKET_LEN);
	g_tx_index = 0u;
}


/* Handles I2C1 events */
void I2C1_EV_IRQHandler(void)
{
	uint32_t isr = I2C1->ISR;

	/* Handle an address match from the master */
	if(isr & I2C_ISR_ADDR)
	{
		/* DIR bit set means the master is trying to read */
		if(isr & I2C_ISR_DIR)
		{

			/* Prepare snapshot and flush any existing bytes in the transmit register */
			I2C1->ISR = I2C_ISR_TXE;

			i2c_take_snapshot();
		}

		/* Clear address flag and refresh ISR */
		I2C1->ICR = I2C_ICR_ADDRCF;
		isr = I2C1->ISR;
	}


	/* Send next byte is the transmit register is ready */
	if(isr & I2C_ISR_TXIS){
		uint8_t b = 0xFFu; // required if the packet length runs out
		if(g_tx_index < GYRO_PACKET_LEN)
		{
			b = g_tx_buf[g_tx_index++];
		}
		I2C1->TXDR = b;
	}

	if(isr & I2C_ISR_NACKF)
	{
		I2C1->ICR = I2C_ICR_NACKCF;
	}

	if(isr & I2C_ISR_STOPF)
	{
		I2C1->ICR = I2C_ICR_STOPCF;
	}
}

/* Handles I2C error events and counts them */
void I2C1_ER_IRQHandler(void)
{
	uint32_t isr = I2C1->ISR;

	/* Invalid start/stop condition detected */
	if(isr & I2C_ISR_BERR)
	{
		I2C1->ICR = I2C_ICR_BERRCF;
		g_i2c_berr++;
	}

	/* Arbitration error */
	if(isr & I2C_ISR_ARLO)
	{
		I2C1->ICR = I2C_ICR_ARLOCF;
		g_i2c_arlo++;
	}

	/* Overrun or underrun error */
	if(isr & I2C_ISR_OVR)
	{
		I2C1->ICR = I2C_ICR_OVRCF;
		g_i2c_ovr++;
	}
}
