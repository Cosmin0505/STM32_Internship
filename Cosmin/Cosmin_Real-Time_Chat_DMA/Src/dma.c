/*
 * dma.c
 *
 *  Created on: Aug 5, 2026
 *      Author: busilam
 */


#include "stm32f070xb.h"
#include "dma.h"
#include <string.h>

/* Initialize DMA Channel 4 for Nucleo <-> Laptop peripheric */
void dma4_init(void) {
	/* Deactivate Channel 4 */
	DMA1_Channel4->CCR &=~ EN;
	/* Enable clock register for DMA */
	RCC->AHBENR |= DMAEN;
	/* Clear interrupts flags */
	DMA1->IFCR |= CGIF4;
	/* Select channel 4 for USART2 TX*/
	/* Get the UART2->TDR */
	DMA1_Channel4->CPAR = (uint32_t)(&(USART2->TDR));
	/* Set memory address */
//	DMA1_Channel4->CMAR = (uint32_t)msg;
	/* Configure no. data to transfer in CNDTR: */
//	DMA1_Channel4->CNDTR = strlen(msg);
	/* Configure parameters */
	/* Channel priority */
	/* Leave 0 as default */
	/* Data transfer direction */
	DMA1_Channel4->CCR |= DIR;
//	/* Circular mod: */
//	DMA1_Channel4->CCR |= CIRC;
	/* Peripheral and memory incremented mode */
	DMA1_Channel4->CCR |= MIRC;
	/* Peripheral and memory data size */
	/* 0 default 8 bits */
	/* Interrupt enable at half and/or full transfer and/or transfer error */
	DMA1_Channel4->CCR |= TCIE;
	DMA1->ISR |= TCIF4;
	/* Enable UART2 transmitter DMA */
	USART2->CR3 |= DMAT;
	NVIC_EnableIRQ(DMA1_Channel4_5_IRQn);
	/* Activate channel 4 */
	DMA1_Channel4->CCR |= EN;
}

/* Initialize DMA Channel 2 for Nucleo <-> Discovery peripheric */
void dma2_init(void) {
	/* Deactivate Channel 4 */
	DMA1_Channel2->CCR &=~ EN;
	/* Enable clock register for DMA */
	RCC->AHBENR |= DMAEN;
	/* Clear interrupts flags */
	DMA1->IFCR |= CGIF2;
	/* Select channel 4 for USART2 TX*/
	/* Get the UART2->TDR */
	DMA1_Channel2->CPAR = (uint32_t)(&(USART1->TDR));
	/* Set memory address */
//	DMA1_Channel2->CMAR = (uint32_t)msg;
	/* Configure no. data to transfer in CNDTR: */
//	DMA1_Channel2->CNDTR = strlen(msg);
	/* Configure parameters */
	/* Channel priority */
	/* Leave 0 as default */
	/* Data transfer direction */
	DMA1_Channel2->CCR |= DIR;
//	/* Circular mod: */
//	DMA1_Channel2->CCR |= CIRC;
	/* Peripheral and memory incremented mode */
	DMA1_Channel2->CCR |= MIRC;
	/* Peripheral and memory data size */
	/* 0 default 8 bits */
	/* Interrupt enable at half and/or full transfer and/or transfer error */
	DMA1_Channel2->CCR |= TCIE;
	DMA1->ISR |= TCIF4;
	/* Enable UART2 transmitter DMA */
	USART1->CR3 |= DMAT;
	NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
	/* Activate channel 4 */
	DMA1_Channel2->CCR |= EN;
}

void DMA1_CH4_5_IRQHandler(void) {
	/* Check for transfer complete interrupt */
	if(DMA1->ISR & TCIF4) {
		/* Clear flag */
		DMA1->IFCR |= CTCIF4;
	}
}

void DMA1_CH2_3_IRQHandler(void) {
	/* Check for transfer complete interrupt */
	if(DMA1->ISR & TCIF2) {
		/* Clear flag */
		DMA1->IFCR |= CTCIF2;
	}
}

void dma4_write(uint32_t msg, uint32_t len) {
	/* Deactivate Channel 4 */
	DMA1_Channel4->CCR &=~ EN;
	/* Clear interrupts flags */
	DMA1->IFCR |= CGIF4;
	/* Configure no. data to transfer in CNDTR: */
	DMA1_Channel4->CNDTR = len;
	/* Set memory address */
	DMA1_Channel4->CMAR = msg;
	/* Activate channel 4 */
	DMA1_Channel4->CCR |= EN;
}

void dma2_write(uint32_t msg, uint32_t len) {
	/* Deactivate Channel 4 */
	DMA1_Channel2->CCR &=~ EN;
	/* Clear interrupts flags */
	DMA1->IFCR |= CGIF4;
	/* Configure no. data to transfer in CNDTR: */
	DMA1_Channel2->CNDTR = len;
	/* Set memory address */
	DMA1_Channel2->CMAR = msg;
	/* Activate channel 4 */
	DMA1_Channel2->CCR |= EN;
}



