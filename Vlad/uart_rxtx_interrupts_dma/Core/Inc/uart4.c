/*
 * uart4.c
 *
 *  Created on: Jul 30, 2026
 *      Author: savav
 */

#include "uart4.h"
#include "usart1.h"
#include <stdbool.h>
#include <string.h>

#define GPIOCEN (1U << 19)

#define DMA2EN (1U << 1)
#define CCR5MINC (1U << 7)
#define CCR5DIR	(1U << 4)
#define CCR5TCIE (1U << 1)

#define UART4EN (1U << 19)
#define APB1CLK 24000000UL
#define BAUD_RATE 9600UL
#define UART4_UE 1U
#define UART4_RE (1U << 2)
#define UART4_TE (1U << 3)
#define MAX_LENGTH 64U

static char tx_buf[MAX_LENGTH];

char c_in_uart4 = ' ';
char received_string_uart4[MAX_LENGTH];
char string_to_be_transmitted_uart4[MAX_LENGTH];
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

void dma2_init(void)
{
	/* Enable clock for DMA2 */
	RCC->AHBENR |= DMA2EN;

	/* Disable Channel 5 */
	DMA2_Channel5->CCR &= ~DMA_CCR_EN;

	/* Clear all error flags for Channel 5 */
	DMA2->IFCR |= DMA_IFCR_CGIF5;
	DMA2->IFCR |= DMA_IFCR_CTCIF5;
	DMA2->IFCR |= DMA_IFCR_CHTIF5;
	DMA2->IFCR |= DMA_IFCR_CTEIF5;

	/* Set destination buffer */
	DMA2_Channel5->CPAR = (uint32_t)&UART4->TDR;

	/* Set source buffer */
	DMA2_Channel5->CMAR = (uint32_t)&string_to_be_transmitted_uart4;

	/* Set length */
//	DMA2_Channel5->CNDTR = MAX_LENGTH;

	/* Enable memory increment */
	DMA2_Channel5->CCR |= CCR5MINC;

	/* Set memory to peripheral direction */
	DMA2_Channel5->CCR |= CCR5DIR;

	/* Enable DMA transfer complete interrupt */
//	DMA2_Channel5->CCR |= CCR5TCIE;

	/* Set PSIZE = 8 bit*/
	DMA2_Channel5->CCR &= ~(3U << 8);

	/* Set MSIZE = 8 bit*/
	DMA2_Channel5->CCR &= ~(3U << 10);

	/* Enable DMA transmitter in USART1 */
	UART4->CR3 |= USART_CR3_DMAT;

	/* Enable Channel 4 */
//	DMA2_Channel5->CCR |= DMA_CCR_EN;

	/* Enable DMA interrupt in NVIC */
//	NVIC_EnableIRQ(DMA2_Channel5_IRQn);
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

bool uart4_tx_busy(void)
{
	return (DMA2_Channel5->CNDTR != 0U);
}

bool uart4_write(const char *s)
{
	if(uart4_tx_busy())
		return false;

	uint16_t len = strlen(s);

	if(len == 0U)
		return false;

	memcpy(tx_buf, s, len);



	DMA2_Channel5->CCR &= ~DMA_CCR_EN;

	DMA2->IFCR |= DMA_IFCR_CGIF5;
	DMA2->IFCR |= DMA_IFCR_CTCIF5;
	DMA2->IFCR |= DMA_IFCR_CHTIF5;
	DMA2->IFCR |= DMA_IFCR_CTEIF5;

	DMA2_Channel5->CMAR = (uint32_t)tx_buf;
	DMA2_Channel5->CNDTR = len;
	DMA2_Channel5->CCR |= DMA_CCR_EN;

	return true;
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
	if(c_in_uart4 != '\r')
	{
		string_to_be_transmitted_uart4[len_uart4++] = c_in_uart4;
	}
	else
	{
		string_to_be_transmitted_uart4[len_uart4++] = '\r';
		string_to_be_transmitted_uart4[len_uart4] = '\0';
		usart1_write(string_to_be_transmitted_uart4);
		usart1_write("\r\n");
		len_uart4 = 0U;
		string_to_be_transmitted_uart4[0] = '\0';
	}
}



void UART4_IRQHandler(void)
{
	uart4_rx_interrupt_test_handler();
}

