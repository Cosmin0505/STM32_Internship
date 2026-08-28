/*
 * ********************************************************************
 * @file		stm32f07xx.c
 * @author		Cosmin
 * @brief		Collection of applications and initializations
 * 				for peripherals used for STM32F070RB Nucleo Board.
 *
 * ********************************************************************
 */

#include "stm32f070xb.h"
#include "stm32f07xx.h"

/* UART2 - Communication Nucleo <-> Laptop */
void uart2_init(void) {
	/* Configura PA2 as UART2 TX */
	/* Enable clock access to GPIOA */
	RCC->AHBENR |= GPIOA_EN;
	/* Configure PA2 as alternate mode */
	GPIOA->MODER &=~ (1U << 4);
	GPIOA->MODER  |= (1U << 5);
	/* Set PA2 alternate function type */
	GPIOA->AFR[0] |= (1U << 8);
	GPIOA->AFR[0] &=~ (1U << 9);
	GPIOA->AFR[0] &=~ (1U << 10);
	GPIOA->AFR[0] &=~ (1U << 11);

	/* Configura PA2 as UART2 RX */
	/* Configure PA3 as alternate mode */
	GPIOA->MODER &=~ (1U << 6);
	GPIOA->MODER  |= (1U << 7);
	/* Set PA3 alternate function type */
	GPIOA->AFR[0] |= (1U << 12);
	GPIOA->AFR[0] &=~ (1U << 13);
	GPIOA->AFR[0] &=~ (1U << 14);
	GPIOA->AFR[0] &=~ (1U << 15);

	/* Enable clock access to UART2 and UART3 */
	RCC->APB1ENR = UART2_EN;
	/* Configure UART2 */
	/* Enable Receiver Configuration (p. 600) */
	/* 1. Program M bit in USART2->CR1 */
	/* (keep it 0 as default 8-bit msg length) */
	/* 2. Select desired baudarte (9600) */
	USART2->BRR = (FCLK + (BAUDRATE/2))/BAUDRATE;
	/* 3. Program no. stop bits in USART_CR2 */
	/* (keep it 0 as default 1 stop bit) */
	/* 4. Enable interruption enable on receive in USART_CR1 */
	USART2->CR1 |= UART2_RXNEIE;
	/* 5. Enable UART by setting UE bit in USART_CR1 */
	/* USART2->CR1 |= UART2_UE; */
	/* Enable Receiver */
	USART2->CR1 |= UART2_RE;

	/* Enable Transmitter Configuration (p. 597) */
	/* 1. Program the M bit in USART_CR1 to define the msg length */
	/* Keep 0 as default 8-bit msg length */
	/* 2. Select the desired baud rate using the USART_BRR register */
	USART2->BRR = (FCLK + (BAUDRATE/2))/BAUDRATE;
	/* 3. Program the number of stop bits in USART_CR2 */
	/* (keep it 0 as default 1 stop bit) */
	/* 4. Enable the USART by writing the UE bit in USART_CR1 register to 1 */
	USART2->CR1 |= UART2_UE;
	/* Enable Transmitter */
	USART2->CR1 |= UART2_TE;
}

/* UART1 - Communication Nucleo <-> Discovery */
void uart1_init(void) {
	/* Configura PA9 as UART1 TX */
	/* Enable clock access to GPIOA */
	RCC->AHBENR |= GPIOA_EN;
	/* Configure PA9 as alternate mode */
	GPIOA->MODER &=~ (1U << 18);
	GPIOA->MODER  |= (1U << 19);
	/* Set PA9 alternate function type */
	GPIOA->AFR[1] |= (1U << 4);
	GPIOA->AFR[1] &=~ (1U << 5);
	GPIOA->AFR[1] &=~ (1U << 6);
	GPIOA->AFR[1] &=~ (1U << 7);

	/* Configura PA10 as UART1 RX */
	/* Configure PA10 as alternate mode */
	GPIOA->MODER &=~ (1U << 20);
	GPIOA->MODER  |= (1U << 21);
	/* Set PA10 alternate function type */
	GPIOA->AFR[1] |= (1U << 8);
	GPIOA->AFR[1] &=~ (1U << 9);
	GPIOA->AFR[1] &=~ (1U << 10);
	GPIOA->AFR[1] &=~ (1U << 11);

	/* Enable clock access to UART1 */
	RCC->APB2ENR |= UART1_EN;
	/* Configure UART1 */
	/* Enable Receiver Configuration (p. 600) */
	/* 1. Program M bit in USART2->CR1 */
	/* (keep it 0 as default 8-bit msg length) */
	/* 2. Select desired baudarte (9600) */
	USART1->BRR = (FCLK + (BAUDRATE/2))/BAUDRATE;
	/* 3. Program no. stop bits in USART_CR2 */
	/* (keep it 0 as default 1 stop bit) */
	/* 4. Enable interruption enable on receive in USART_CR1 */
	USART1->CR1 |= UART1_RXNEIE;
	/* 5. Enable UART by setting UE bit in USART_CR1 */
	/* USART2->CR1 |= UART2_UE; */
	/* Enable Receiver */
	USART1->CR1 |= UART1_RE;

	/* Enable Transmitter Configuration (p. 597) */
	/* 1. Program the M bit in USART_CR1 to define the msg length */
	/* Keep 0 as default 8-bit msg length */
	/* 2. Select the desired baud rate using the USART_BRR register */
	USART1->BRR = (FCLK + (BAUDRATE/2))/BAUDRATE;
	/* 3. Program the number of stop bits in USART_CR2 */
	/* (keep it 0 as default 1 stop bit) */
	/* 4. Enable the USART by writing the UE bit in USART_CR1 register to 1 */
	USART1->CR1 |= UART1_UE;
	/* Enable Transmitter */
	USART1->CR1 |= UART1_TE;
}

void uart2_write(char bit) {
	/* Wait until transmission is complete */
	while(!((USART2->ISR & UART_TXE) && (USART2->ISR & UART2_TC))) {
	}
	/* If TC bit is set, store into TDR the value */
	if(USART2->ISR & UART2_TC) {
		USART2->TDR = (uint16_t)bit;
	}
}

void uart2_write_string(char* received_msg) {
	for(int i=0; i<strlen(received_msg); i++) {
	    		uart2_write(received_msg[i]);
			}
}

void uart2_read(char *output) {
	/* Wait until receive is complete */
	while(!(USART2->ISR & UART2_RXNE)) {
	}
	*output = USART2->RDR;
}

void uart1_write(char bit) {
	/* Wait until transmission is complete */
	while(!((USART1->ISR & UART_TXE) && (USART1->ISR & UART1_TC))) {
	}
	/* If TC bit is set, store into TDR the value */
	if(USART1->ISR & UART1_TC) {
		USART1->TDR = (uint16_t)bit;
	}
}


void uart1_read(char *output) {
	/* Idee: Stochez ce primesc intr-un string, si afisez pe ecran cand primesc valoarea enter */
	/* Wait until receive is complete */
	while(!(USART1->ISR & UART1_RXNE)) {
	}
	*output = USART1->RDR;
}

void transmit(void) {
	/* Initiate message and bit to transmit */
	char bit[2] = "\0\0";
	char msg[100] = "\0";
	/* Read from keyboard each bit I want to send to Discovery */
	/* Store each bit I write in a buffer untill I press enter */
	do {
		uart2_read(&bit[0]);
		/* Display to prompt each bit I type */
		uart2_write(bit[0]);
		strcat(msg, bit);
	} while(bit[0] != '\r');
	for(int i=0; i<strlen(msg); i++) {
		/* Send bit to Discovery */
		uart1_write(msg[i]);
	}
	/* Send new line to Discovery */
	uart1_write('\n');
	/* Write to terminal */
	uart2_write_string("\rCosmin: ");
	uart2_write_string(msg);
	uart2_write_string("\n\r");
}

void transmit_debug(char *send_msg) {
	for(int i=0; i<strlen(send_msg); i++) {
		/* Send bit to Discovery */
		uart1_write(send_msg[i]);
	}
	strcat("Cosmin: ", send_msg);
	uart2_write_string(send_msg);
}


int8_t receive(char *received_bit, char *received_msg) {
	/* Set return flag */
	int8_t switch_to_write = 0;
	/* Read from Discovery */
	uart1_read(&received_bit[0]);
	strcat(received_msg, received_bit);
	/* If Enter received from Discovery */
	if(received_bit[0] == '\n') {
		/* Write to terminal */
		uart2_write_string("\r\nVlad: ");
		uart2_write_string(received_msg);
		uart2_write_string("\r\n");
		/* Reset message buffer */
		strcpy(received_msg, "\0");
		/* Enable writing */
		switch_to_write = 1;
	}
	return switch_to_write;
}

/*
 * Functions declaration for I2C peripheral
 */
/* Configure I2C peripheral as following:
 * NUCLEO - MASTER
 * DISCOVERY - SLAVE
 * Scope - Demand data from Discovery gyroscope
 */

/* Little endian decoder */
static int32_t get_le32(const uint8_t *source) {
	/* Return decoded value */
	uint32_t value = (uint32_t)source[0];
	value |= (uint32_t)source[1] << 8U;
	value |= (uint32_t)source[2] << 16U;
	value |= (uint32_t)source[3] << 24U;
	return (int32_t)value / 1000;
}

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
	/* Set pull-up configuration */
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
	/* Set timing register */
	I2C2->TIMINGR = 0x10420F13U;
	/* Set slave address, read transfer, autoend */
	I2C2->CR2 = 0U;
	/* Clear all flags */
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

static void i2c2_start_xyz_read(void) {
	/* Skip request if another I2C transaction is in progress */
	if(I2C2->ISR & I2C_ISR_BUSY) {
		return;
	}
	g_rx_error = 0U;
	/* Clear flags from previous transactions */
	I2C2->ICR = I2C_ICR_STOPCF |
			    I2C_ICR_NACKCF |
				I2C_ICR_BERRCF |
				I2C_ICR_ARLOCF |
				I2C_ICR_OVRCF;
	/* Configure slave address, read request (receive data from Discovery), number of bytes to receive
	 * and set AUTOEND to automatically generate STOP after receiving all bytes.*/
	I2C2->CR2 = ((uint32_t)DISCOVERY_I2C_ADDR_7BIT << 1U) |
				I2C_CR2_RD_WRN |
				((uint32_t)GYRO_XYZ_LEN << 16U) |
				I2C_CR2_AUTOEND;
	/* Start transaction */
	I2C2->CR2 |= I2C_CR2_START;
}

void EXTI4_15_IRQHandler(void) {
	/* Clear EXTI flag */
	EXTI->PR |= EXTI_PR_PIF13;
	/* Start I2C slave transmission (master read) sequence */
	i2c2_start_xyz_read();
}

void I2C2_IRQHandler(void) {
	uint32_t isr = I2C2->ISR;
	static uint8_t g_rx_buf[GYRO_XYZ_LEN];
	static uint8_t g_valid_xyz_count;
	static uint8_t g_rx_index;
	static uint8_t g_rx_error;

	/* When one new byte arrived from the Discovery */
	if((isr & I2C_ISR_RXNE)) {
		/* Store received bit */
		uint8_t received = (uint8_t)I2C2->RXDR;
		/* Store the byte if it does not exceed buffer size */
		if(g_rx_index < GYRO_XYZ_LEN) {
			/* Put in the buffer */
			g_rx_buf[g_rx_index++] = received;
		} else {
			/* More bytes arrived than expected */
			g_rx_error = 1U;
		}
		/* Refresh ISR, as reading the last byte triggers STOPF */
		isr = I2C2->ISR;
	}


	/* The I2C transaction finished */
	if(isr & I2C_ISR_STOPF) {
		/* Clear stop receive flag */
		I2C2->ICR = I2C_ICR_STOPCF;
		if((g_rx_error == 0U) && (g_rx_index == GYRO_XYZ_LEN)) {
			/* Convert each little endian group */
			int32_t g_gyro_x_dps;
			int32_t g_gyro_y_dps;
			int32_t g_gyro_z_dps;
			g_gyro_x_dps = get_le32(&g_rx_buf[0]);
			g_gyro_y_dps = get_le32(&g_rx_buf[4]);
			g_gyro_z_dps = get_le32(&g_rx_buf[8]);
			/* Increment counter */
			g_valid_xyz_count++;
			/* Toggle LED on each successful transfer */
			GPIOA->ODR ^= LED_PIN;
		}
	}

	/* Set error flag if one of the following error occurs */
	if(isr & I2C_ISR_NACKF) { /* Not acknowledged */
		I2C2->ICR = I2C_ICR_NACKCF;
		g_rx_error = 1U;
	} else if ((isr & I2C_ISR_BERR) != 0U) { /* Bus error */
		I2C2->ICR = I2C_ICR_BERRCF;
		g_rx_error = 1U;
	} else if ((isr & I2C_ISR_ARLO) != 0U) { /* Arbitration lost */
		I2C2->ICR = I2C_ICR_ARLOCF;
		g_rx_error = 1U;
	} else {
		/* No error received */
	}
}
