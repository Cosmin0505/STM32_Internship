/*
 * uart4.c
 *
 *  Created on: Jul 30, 2026
 *      Author: savav
 */

#include "uart4.h"

#define GPIOCEN (1 << 19)
#define UART4EN (1 << 19)
#define APB1CLK 24000000UL
#define BAUD_RATE 9600UL
#define UART4_UE 1U
#define UART4_RE (1U << 2)
#define UART4_TE (1U << 3)

void uart4_init(void)
{
	/* Enable GPIOC clock */
	RCC->AHBENR |= GPIOCEN;

	/* Enable UART4 clock */
	RCC->APB1ENR |= UART4EN;

	/* Configure Pins PC10 and PC11 in Alternate function mode */
	GPIOC->MODER &= ~((3U << (10 * 2)) | (3U << (11 * 2)));
	GPIOC->MODER |= ((2U << (10 * 2)) | (2U << (11 * 2)));


	/* Configure alternate functions for GPIO pins*/
	GPIOC->AFR[1] &= ~((15U << ((10 - 8) * 4)) | (15U << ((11 - 8) * 4)));
	GPIOC->AFR[1] |= ((5U << ((10 - 8) * 4)) | (5U << ((11 - 8) * 4)));


	/* Configure pins to High Speed */
	GPIOC->OSPEEDR &= ~((3U << (10 * 2)) | (3U << (11 * 2)));
	GPIOC->OSPEEDR |= ((3U << (10 * 2)) | (3U << (11 * 2)));

	/* Configure pins output type */
	GPIOC->OTYPER &= ~((1U << 10) | (1U << 11));

	GPIOC->PUPDR &= ~((3U << (10 * 2)) | (3U << (11 * 2)));
	GPIOC->PUPDR |= (1U << (11 * 2));

	/* Configure oversampling of 16 and disable UART during set-up*/
	UART4->CR1 = 0;

	/* Configure the other config registers with default settings */
	UART4->CR2 = 0;
	UART4->CR3= 0;

	/* Configure baud rate */
	UART4->BRR = (APB1CLK + BAUD_RATE / 2) / BAUD_RATE;

	/* Enable UART, transmission and reception */
	UART4->CR1 |= UART4_UE | UART4_RE | UART4_TE;

	while((UART4->ISR & USART_ISR_TEACK) == 0U);
	while((UART4->ISR & USART_ISR_REACK) == 0U);

}

void uart4_transmit_byte(char c){
	while((UART4->ISR & USART_ISR_TC) == 0U);
	UART4->TDR = c;
}

void uart4_receive_byte(char *c){
	while((UART4->ISR & USART_ISR_RXNE) == 0U);

	/* Technically redundant check */
	*c = (char)(UART4->RDR);
}

bool uart4_try_receive_byte(char *c){
	if((UART4->ISR & USART_ISR_RXNE)  != 0)
	{
		*c = (char)(UART4->RDR);
		return true;
	}
	return false;
}

void uart4_transmit_string(char s[])
{
	uint8_t len = strlen(s);
	for(int i = 0; i < len; i++)
	{
		uart4_transmit_byte(s[i]);
	}
}

