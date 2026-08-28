/*
 * uart.h
 *
 *  Created on: Jul 30, 2026
 *      Author: busilam
 */

#include "stm32f070xb.h"

#ifndef UART_H_
#define UART_H_

#define VLAD_STRING		"Vlad: "

#define UART_TXE		(1U << 7)

#define	UART2_EN		(1U << 17)
#define	UART2_TE		(1U << 3)
#define UART2_RE		(1U << 2)
#define UART2_UE		(1U << 0)
#define UART2_RXNEIE	(1U << 5)
#define	UART2_RXNE		UART2_RXNEIE
#define UART2_TC		(1U << 6)
#define UART2_TCCF		UART2_TC

#define	UART1_EN		(1U << 14)
#define	UART1_TE		UART2_TE
#define UART1_RE		UART2_RE
#define UART1_UE		UART2_UE
#define UART1_RXNEIE	UART2_RXNEIE
#define	UART1_RXNE		UART2_RXNE
#define UART1_TC		UART2_TC
#define UART1_TCIE		UART2_TC
#define UART1_TCCF		UART2_TCCF

#define	BAUDRATE		9600
#define FCLK			8000000

#define GPIOA_EN		UART2_EN

#define MAX_SIZE	100

void uart1_init(void);
void uart1_read(char *output);
void uart1_write(char bit);

void uart2_init(void);
void uart2_read(char *output);
void uart2_write(char bit);
void uart2_write_string(char* received_msg);

void transmit(void);
void transmit_debug(char *send_msg);
int8_t receive(char *received_char, char *received_msg);

void build_message(char *send_msg, char bit, uint8_t *len);

#endif /* UART_H_ */
