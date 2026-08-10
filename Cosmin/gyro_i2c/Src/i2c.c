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

void gpio_i2c1_init_slave(void) {
	/* Pins to be configured PB6 & PB7 */
	/* Enable clock peripheral for GPIOB */
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	/* Set open drain for I2C */
	GPIOB->OTYPER |= (GPIO_OTYPER_OT_6) | (GPIO_OTYPER_OT_7);
	/* Set AF Mode */
	GPIOB->MODER |= (GPIO_MODER_MODER6_1) | (GPIO_MODER_MODER7_1);
	/* Select AF1 */
	GPIOB->AFR[0] = GPIOB->AFR[0] &~ (GPIO_AFRL_AFSEL6 | GPIO_AFRL_AFSEL7);
	GPIOB->AFR[0] |= (1U << GPIO_AFRL_AFSEL6_Pos) |	(1U << GPIO_AFRL_AFSEL7_Pos);
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR6_0;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR7_0;
}

/* Slave */
void i2c1_init(void) {
	/* Enable clock peripheral for I2C1 */
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	/* Use sysclk for I2C Clk */
	RCC->CFGR3 |= RCC_CFGR3_I2C1SW;
	/* Configure I2C1 as slave */
	/* Timing register value Timing register value is computed with the AN4235 xls file,
         fast Mode @400kHz with I2CCLK = 48MHz, rise time = 140ns,
         fall time = 40ns */
	I2C1->TIMINGR = (uint32_t)0x00B00000;
	/* Enable I2C1 */
	I2C1->CR1 |= I2C_CR1_PE;
	/* Enable address match interrupt */
	I2C1->CR1 |= I2C_CR1_ADDRIE;
	/* Set own address */
	I2C1->OAR1 = 0;
	I2C1->OAR1 |= (I2C1_OWN_ADDRESS << 1);
	/* Enable own address */
	I2C1->OAR1 |= I2C_OAR1_OA1EN;
	/* Configure interrupt */
	/* Set priority to I2C1_IRQn */
	NVIC_SetPriority(I2C1_IRQn, 0);
	/* Enable interrupt */
	NVIC_EnableIRQ(I2C1_IRQn);

}

void gpio_i2c2_init_master(void) {
	/* Pins to be configured PB10 & PB11 */
	/* Enable clock peripheral for GPIOB */
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	/* Set open drain for I2C */
	GPIOB->OTYPER |= (GPIO_OTYPER_OT_10) | (GPIO_OTYPER_OT_11);
	/* Set AF Mode */
	GPIOB->MODER = 0U;
	GPIOB->MODER |= (GPIO_MODER_MODER10_1) | (GPIO_MODER_MODER11_1);
	/* Select AF1 */
	GPIOB->AFR[1] = GPIOB->AFR[1] &~ (GPIO_AFRH_AFSEL10 | GPIO_AFRH_AFSEL11);
	GPIOB->AFR[1] |= (1U << GPIO_AFRH_AFSEL10_Pos) | (1U << GPIO_AFRH_AFSEL11_Pos);
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
	I2C2->TIMINGR = (uint32_t)0x00B01A4B;
	/* Set slave address, read transfer, 1 byte to receive, autoend */
	I2C2->CR2 &= ~I2C_CR2_SADD;
	I2C2->CR2 |= (I2C1_OWN_ADDRESS << 1);
	I2C2->CR2 |= I2C_CR2_RD_WRN;
	I2C2->CR2 |= I2C_CR2_AUTOEND;
	I2C2->CR2 |= (1U << 16);
	/* Enable I2C2 */
	I2C2->CR1 |= I2C_CR1_PE;
	/* Enable receive interrupt */
	I2C2->CR1 |= I2C_CR1_RXIE;
	/* Configure interrupt */
	/* Set priority to I2C1_IRQn */
	NVIC_SetPriority(I2C2_IRQn, 0);
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

void EXTI4_15_IRQHandler(void) {
	GPIOA->ODR ^= LED_PIN; // temporary
	/* Clear EXTI flag */
	EXTI->PR |= EXTI_PR_PIF13;
	/* Start I2C slave transmission sequence */
	I2C2->CR2 |= I2C_CR2_START;

	volatile uint32_t isr_master = I2C2->ISR;
	volatile uint32_t isr_slave = I2C1->ISR;
}

void I2C1_IRQHandler(void) {
  uint32_t I2C_InterruptStatus = I2C1->ISR; /* Get interrupt status */

  if((I2C_InterruptStatus & I2C_ISR_ADDR) == I2C_ISR_ADDR) /* Check address match */ {
    I2C1->ICR |= I2C_ICR_ADDRCF; /* Clear address match flag */
    if((I2C1->ISR & I2C_ISR_DIR) == I2C_ISR_DIR) /* Check if transfer direction is read (slave transmitter) */ {
      I2C1->CR1 |= I2C_CR1_TXIE; /* Set transmit IT */
    }
  } else if((I2C_InterruptStatus & I2C_ISR_TXIS) == I2C_ISR_TXIS) {
    I2C1->CR1 &=~ I2C_CR1_TXIE; /* Disable transmit IT */
    I2C1->TXDR = I2C_BYTE_TO_SEND; /* Byte to send */
  } else {
  }
}

void I2C2_IRQHandler(void) {
	uint32_t I2C_InterruptStatus = I2C2->ISR; /* Get interrupt status */
    if ((I2C_InterruptStatus & I2C_ISR_ADDR) == I2C_ISR_ADDR) {
    	I2C2->ICR |= I2C_ICR_ADDRCF; /* Clear address match flag */
    } else if ((I2C_InterruptStatus & I2C_ISR_RXNE) == I2C_ISR_RXNE) { /* Check if receive data reg not empty */
        if (I2C2->RXDR == I2C_BYTE_TO_SEND) { /* If we receive the right byte */
            GPIOA->ODR ^= LED_PIN; /* Toggle LED */
        }
    } else {
    }
}



