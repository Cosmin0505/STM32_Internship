/*
 * usart1.c
 *
 *  Created on: Jul 30, 2026
 *      Author: savav
 */

#include "usart1.h"
#include <string.h>
#include "uart4.h"
#include <stdbool.h>

#define GPIOCEN (1U << 19)
#define USART1EN (1U << 14)

#define DMA1EN (1U << 0)
#define CCR4MINC (1U << 7)
#define CCR4DIR (1U << 4)
#define CCR4TCIE (1U << 1)

#define APB2CLK 48000000UL /* 48 MHz PCLK2 frequency */
#define BAUD_RATE 9600UL
#define USART1_UE 1U
#define USART1_RE (1U << 2)
#define USART1_TE (1U << 3)
#define MAX_LENGTH 64U



static char tx_buf[MAX_LENGTH];

char c_in = ' ';
char pc_received_string[MAX_LENGTH];
char string_to_be_transmitted[MAX_LENGTH] = "a";
uint8_t len = 0U;

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


	/* Generate an interrupt every time something is received */
	USART1->CR1 |= USART_CR1_RXNEIE;

	/* Enable UART, transmission and reception */
	USART1->CR1 |= USART1_UE | USART1_RE | USART1_TE;

	while((USART1->ISR & USART_ISR_TEACK) == 0U);
	while((USART1->ISR & USART_ISR_REACK) == 0U);

	NVIC_SetPriority(USART1_IRQn, 2);
	NVIC_EnableIRQ(USART1_IRQn);
}


void dma1_init(void)
{
	/* Enable clock for DMA1 */
	RCC->AHBENR |= DMA1EN;

	/* Disable Channel 4 */
	DMA1_Channel4->CCR &= ~DMA_CCR_EN;

	/* Clear all error flags for Channel 4 */
	DMA1->IFCR |= DMA_IFCR_CGIF4;
	DMA1->IFCR |= DMA_IFCR_CTCIF4;
	DMA1->IFCR |= DMA_IFCR_CHTIF4;
	DMA1->IFCR |= DMA_IFCR_CTEIF4;

	/* Set destination buffer */
	DMA1_Channel4->CPAR = (uint32_t)&USART1->TDR;

	/* Set source buffer */
	DMA1_Channel4->CMAR = (uint32_t)&string_to_be_transmitted;

	/* Set length */
//	DMA1_Channel4->CNDTR = MAX_LENGTH;

	/* Enable memory increment */
	DMA1_Channel4->CCR |= CCR4MINC;

	/* Set memory to peripheral direction */
	DMA1_Channel4->CCR |= CCR4DIR;

	/* Enable DMA transfer complete interrupt */
//	DMA1_Channel4->CCR |= CCR4TCIE;

	/* Set PSIZE = 8 bit*/
	DMA1_Channel4->CCR &= ~(3U << 8);

	/* Set MSIZE = 8 bit*/
	DMA1_Channel4->CCR &= ~(3U << 10);

	/* Enable DMA transmitter in USART1 */
	USART1->CR3 |= USART_CR3_DMAT;

	/* Enable Channel 4 */
//	DMA1_Channel4->CCR |= DMA_CCR_EN;

	/* Enable DMA interrupt in NVIC */
//	NVIC_EnableIRQ(DMA1_Channel4_IRQn);
}


void usart1_transmit_byte(char c){
	if((USART1->ISR & USART_ISR_TC))
	{
		USART1->TDR = c;
	}

}

void usart1_receive_byte(char *c){
//	while((USART1->ISR & USART_ISR_RXNE) == 0U);

	if((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE)
	{
		*c = (char)(USART1->RDR);
	}
}

void usart1_transmit_string(char s[])
{
	uint8_t len = strlen(s);
	for(int i = 0; i < len; i++)
	{
		usart1_transmit_byte(s[i]);
	}
}

bool usart1_tx_busy(void)
{
	return (DMA1_Channel4->CNDTR != 0U);
}

bool usart1_write(const char *s)
{
	if(usart1_tx_busy())
		return false;

	uint16_t len = strlen(s);

	if(len == 0U)
		return false;

	memcpy(tx_buf, s, len);



	DMA1_Channel4->CCR &= ~DMA_CCR_EN;

	DMA1->IFCR |= DMA_IFCR_CGIF4;
	DMA1->IFCR |= DMA_IFCR_CTCIF4;
	DMA1->IFCR |= DMA_IFCR_CHTIF4;
	DMA1->IFCR |= DMA_IFCR_CTEIF4;

	DMA1_Channel4->CMAR = (uint32_t)tx_buf;
	DMA1_Channel4->CNDTR = len;
	DMA1_Channel4->CCR |= DMA_CCR_EN;

	return true;
}

void usart1_rx_interrupt_handler(void)
{
	usart1_receive_byte(&c_in);

	if((c_in == '\r'))
		{
			if(len > 0)
			{

				/* \r is the chosen terminator for a sent message*/
				pc_received_string[len++] = '\r';
				pc_received_string[len] = '\0';
				uart4_transmit_string(pc_received_string);
				usart1_transmit_byte('\r');
				usart1_transmit_string("Vlad: ");
				usart1_transmit_string(pc_received_string);
				usart1_transmit_string("\r\n");
				len = 0U;
				return;
			}

		}
		else
		{
			if(len < (MAX_LENGTH - 1))
			{
				pc_received_string[len++] = c_in;
				usart1_transmit_byte(c_in);
			}
		}
}

void usart1_rx_interrupt_test_handler(void)
{
	usart1_receive_byte(&c_in);
	if(c_in == '\r')
	{
		string_to_be_transmitted[len++] = '\r';
		string_to_be_transmitted[len] = '\0';
//		usart1_write(string_to_be_transmitted);
		uart4_write(string_to_be_transmitted);
		len = 0U;
		string_to_be_transmitted[0] = '\0';
	}
	else
	{
		string_to_be_transmitted[len++] = c_in;
	}
}



void USART1_IRQHandler(void)
{
	usart1_rx_interrupt_test_handler();
}




