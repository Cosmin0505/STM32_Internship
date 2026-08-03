/*
 * usart1.c
 *
 *  Created on: Jul 30, 2026
 *      Author: savav
 */

#include "usart1.h"
#include "string.h"

#define GPIOCEN (1U << 19)
#define USART1EN (1U << 14)
#define APB2CLK 48000000UL /* 48 MHz PCLK2 frequency */
#define BAUD_RATE 9600UL
#define USART1_UE 1U
#define USART1_RE (1U << 2)
#define USART1_TE (1U << 3)

void usart1_init(void)
{
	/* Enable GPIOC clock */
	RCC->AHBENR |= GPIOCEN;

	/* Enable USART1 clock */
	RCC->APB2ENR |= USART1EN;

	/* Configure Pins PC4 and PC5 in Alternate function mode */
	GPIOC->MODER &= ~((3U << (4 * 2)) | (3U << (5 * 2)));
	GPIOC->MODER |= ((2U << (4 * 2)) | (2U << (5 * 2)));


	/* Configure alternate functions for GPIO pins*/
	GPIOC->AFR[0] &= ~((15U << (4 * 4)) | (15U << (5 * 4)));
	GPIOC->AFR[0] |= ((7U << (4 * 4)) | (7U << (5 * 4)));


	/* Configure pins to High Speed */
	GPIOC->OSPEEDR &= ~((3U << (4 * 2)) | (3U << (5 * 2)));
	GPIOC->OSPEEDR |= ((3U << (4 * 2)) | (3U << (5 * 2)));

	/* Configure pins output type */
	GPIOC->OTYPER &= ~(1U << 4 | 1U << 5);

	GPIOC->PUPDR &= ~((3U << (4 * 2)) | (3U << (5 * 2)));
	GPIOC->PUPDR |= (1U << (5 * 2));

	/* Configure oversampling of 16 and disable UART during set-up*/
	USART1->CR1 = 0;

	/* Configure the other config registers with default settings */
	USART1->CR2 = 0;
	USART1->CR3= 0;

	/* Configure baud rate */
	USART1->BRR = (APB2CLK + BAUD_RATE / 2) / BAUD_RATE;

	/* Enable UART, transmission and reception */
	USART1->CR1 |= USART1_UE | USART1_RE | USART1_TE;

	while((USART1->ISR & USART_ISR_TEACK) == 0U);
	while((USART1->ISR & USART_ISR_REACK) == 0U);
}

void usart1_transmit_byte(char c){
	while((USART1->ISR & USART_ISR_TC) == 0U);
	USART1->TDR = c;
}

void usart1_receive_byte(char *c){
	while((USART1->ISR & USART_ISR_RXNE) == 0U);


	/* Technically redundant check */
	if((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE)
	{
		*c = (char)(USART1->RDR);
	}
}

bool usart1_try_receive_byte(char *c){
	if((USART1->ISR & USART_ISR_RXNE)  != 0)
	{
		*c = (char)(USART1->RDR);
		return true;
	}
	return false;
}

void usart1_transmit_string(char s[])
{
	uint8_t len = strlen(s);
	for(int i = 0; i < len; i++)
	{
		usart1_transmit_byte(s[i]);
	}
}


