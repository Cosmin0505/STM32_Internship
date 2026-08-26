/*
 * usart1.h
 *
 *  Created on: Jul 30, 2026
 *      Author: savav
 */

#ifndef INC_USART1_H_
#define INC_USART1_H_

#include "stm32f3xx.h"
#include "stdbool.h"

void usart1_init(void);
void dma1_init(void);
void usart1_transmit_byte(char c);
void usart1_receive_byte (char *c);
void usart1_transmit_string (char s[]);
bool usart1_write(const char *s);

#endif /* INC_USART1_H_ */
