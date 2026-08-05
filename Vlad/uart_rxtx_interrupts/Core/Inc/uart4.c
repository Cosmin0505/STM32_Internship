/*
 * uart4.c
 *
 *  Created on: Jul 30, 2026
 *      Author: savav
 */

#include "uart4.h"
#include "usart1.h"

#define GPIOCEN (1 << 19)
#define UART4EN (1 << 19)
#define APB1CLK 24000000UL
#define BAUD_RATE 9600UL
#define UART4_UE 1U
#define UART4_RE (1U << 2)
#define UART4_TE (1U << 3)
#define MAX_LENGTH 64U

char c_in_uart4 = ' ';
char received_string_uart4[MAX_LENGTH];
uint8_t len_uart4;

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

	/* Generate an interrupt every time something is received */
	UART4->CR1 |= USART_CR1_RXNEIE;

	/* Enable UART, transmission and reception */
	UART4->CR1 |= UART4_UE | UART4_RE | UART4_TE;

	while((UART4->ISR & USART_ISR_TEACK) == 0U);
	while((UART4->ISR & USART_ISR_REACK) == 0U);

	NVIC_EnableIRQ(UART4_IRQn);
}

void uart4_transmit_byte(char c){
	if((UART4->ISR & USART_ISR_TC))
	{
		UART4->TDR = c;
	}

}

void uart4_receive_byte(char *c){
//	while((UART4->ISR & USART_ISR_RXNE) == 0U);

	if(UART4->ISR & USART_ISR_ORE)
	{
		UART4->ICR |= USART_ICR_ORECF;
	}

	/* Technically redundant check */
	*c = (char)(UART4->RDR);
}

void uart4_transmit_string(char s[])
{
	uint8_t len = strlen(s);
	for(int i = 0; i < len; i++)
	{
		uart4_transmit_byte(s[i]);
	}
}

void uart4_rx_interrupt_handler(void)
{
	uart4_receive_byte(&c_in_uart4);

	/* \r is the chosen terminator for a received message */
	if(c_in_uart4 != '\r')
	{
		if(len_uart4 < MAX_LENGTH - 1)
		{
			received_string_uart4[len_uart4++] = c_in_uart4;
			received_string_uart4[len_uart4] = '\0';
		}
	}
	else
	{
		usart1_transmit_string("Cosmin: ");
		usart1_transmit_string(received_string_uart4);
		usart1_transmit_string("\r\n");
		len_uart4 = 0U;
		received_string_uart4[0] = '\0';
	}
}

void uart4_rx_interrupt_test_handler(void)
{
	uart4_receive_byte(&c_in_uart4);
	usart1_transmit_byte(c_in_uart4);
}



void UART4_IRQHandler(void)
{
	uart4_rx_interrupt_test_handler();
}

