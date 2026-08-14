/*
 * i2c.c
 *
 *  Created on: Aug 10, 2026
 *      Author: busilam
 */

#include "i2c.h"
#include "stm32f070xb.h"

/* Configure I2C peripheral as following:
 * NUCLEO - MASTER
 * DISCOVERY - SLAVE
 * Scope - Demand data from Discovery gyroscope
 */

volatile uint8_t g_received_byte;
volatile uint8_t g_valid_byte_count;
volatile uint8_t g_i2c_error_count;


void gpio_i2c2_init_master(void) {
	/* Pins to be configured PB10 & PB11 */
	/* Enable clock peripheral for GPIOB */
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	/* Set open drain for I2C */
	GPIOB->OTYPER |= (GPIO_OTYPER_OT_10) | (GPIO_OTYPER_OT_11);

	GPIOB->OSPEEDR &= ((3U << (10 * 2U)) | (3U << (11 * 2U)));
	GPIOB->OSPEEDR |= ((2U << (10 * 2U)) | (2U << (11 * 2U)));

	/* Set AF Mode */
	GPIOB->MODER &= ~((3U << (10 * 2U)) | (3U << (11 * 2U)));
	GPIOB->MODER |= (GPIO_MODER_MODER10_1) | (GPIO_MODER_MODER11_1);
	/* Select AF1 */
	GPIOB->AFR[1] &= ~((0xFU << ((10U - 8U) * 4U) | (0xFU << ((11U - 8U) * 4U))));
	GPIOB->AFR[1] |= (1U << GPIO_AFRH_AFSEL10_Pos) | (1U << GPIO_AFRH_AFSEL11_Pos);

	GPIOB->PUPDR &= ~((3U << (10 * 2U)) | (3U << (11 * 2U)));
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR10_0;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR11_0;
}

/* Master */
void i2c2_init(void) {
	/* Enable clock peripheral for I2C2 */
	RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	/* Timing register value Timing register value is computed with the AN4235 xls file,
	         fast Mode @400kHz with I2CCLK = 48MHz, rise time = 140ns,
	         fall time = 40ns */
	I2C2->CR1 &= ~I2C_CR1_PE;
	while((I2C2->CR1 & I2C_CR1_PE))
	{
	}

	I2C2->TIMINGR = 0x10420F13U;
	/* Set slave address, read transfer, 1 byte to receive, autoend */
	I2C2->CR2 = 0U;

	I2C2->ICR = I2C_ICR_STOPCF
			  | I2C_ICR_NACKCF
			  | I2C_ICR_BERRCF
			  | I2C_ICR_ARLOCF
			  | I2C_ICR_OVRCF;

	/* Enable interrupts */
	I2C2->CR1 |= I2C_CR1_RXIE
			   | I2C_CR1_STOPIE
			   | I2C_CR1_NACKIE
			   | I2C_CR1_ERRIE;

	/* Enable I2C2 */
	I2C2->CR1 |= I2C_CR1_PE;

	/* Configure interrupt */
	/* Set priority to I2C2_IRQn */
	NVIC_SetPriority(I2C2_IRQn, 1U);
	/* Enable interrupt */
	NVIC_EnableIRQ(I2C2_IRQn);
}

void gpio_led_init(void) {
	/* Enable clock peripheral for GPIOA */
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	/* Select output mode (01) on PA5 */
	GPIOA->MODER |= GPIO_MODER_MODER5_0;
	/* GPIOA->ODR ^= LED_PIN LED_PIN (1 <<5) */
}

void gpio_button_init(void) {
	/* Enable clock peripheral for GPIOC */
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	/* Select input mode */
	/* 0 is default */
}

void exti_init(void) {
	/* Configure syscfg, exti and nvic for push button PC13 */
	/* PC13 as source input */
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;
	/* Unmask port 13 */
	/* Set rising edge */
	EXTI->IMR |= EXTI_IMR_IM13;
	EXTI->FTSR |= EXTI_FTSR_TR13;
	/* Set priority */
	NVIC_SetPriority(EXTI4_15_IRQn, 0);
	/* Enable EXTI0_1_IRQn */
	NVIC_EnableIRQ(EXTI4_15_IRQn);
}

static void i2c2_start_test_read(void)
{
	if(I2C2->ISR & I2C_ISR_BUSY)
	{
		return;
	}

	I2C2->ICR = I2C_ICR_STOPCF |
			    I2C_ICR_NACKCF |
				I2C_ICR_BERRCF |
				I2C_ICR_ARLOCF |
				I2C_ICR_OVRCF;

	I2C2->CR2 = ((uint32_t)DISCOVERY_I2C_ADDR_7BIT << 1U) |
				I2C_CR2_RD_WRN |
				(1U << 16U) |
				I2C_CR2_AUTOEND;

	I2C2->CR2 |= I2C_CR2_START;
}

void EXTI4_15_IRQHandler(void) {
	/* Clear EXTI flag */
	EXTI->PR |= EXTI_PR_PIF13;
	/* Start I2C slave transmission sequence */
	i2c2_start_test_read();
}

void I2C2_IRQHandler(void) {
	uint32_t isr = I2C2->ISR;

	if((isr & I2C_ISR_RXNE))
	{
		g_received_byte = I2C2->RXDR;

		if(g_received_byte == I2C_TEST_BYTE)
		{
			g_valid_byte_count++;

			GPIOA->ODR ^= LED_PIN;
		}
		else
		{
			g_i2c_error_count++;
		}
	}
	if(isr & I2C_ISR_NACKF)
	{
		I2C2->ICR = I2C_ICR_NACKCF;
		g_i2c_error_count++;
	}
	if(isr & I2C_ISR_STOPF)
	{
		I2C2->ICR = I2C_ICR_STOPCF;
	}
	if ((isr & I2C_ISR_BERR) != 0U)
	{
		I2C2->ICR = I2C_ICR_BERRCF;
		g_i2c_error_count++;
	}

	if ((isr & I2C_ISR_ARLO) != 0U)
	{
		I2C2->ICR = I2C_ICR_ARLOCF;
		g_i2c_error_count++;
	}

	if ((isr & I2C_ISR_OVR) != 0U)
	{
		I2C2->ICR = I2C_ICR_OVRCF;
		g_i2c_error_count++;
	}
}



