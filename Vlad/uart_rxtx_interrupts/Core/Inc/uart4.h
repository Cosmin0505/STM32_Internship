/*
 * uart4.h
 *
 *  Created on: Jul 30, 2026
 *      Author: savav
 */

#ifndef INC_UART4_H_
#define INC_UART4_H_

#include "stm32f3xx.h"
#include "string.h"
#include "stdbool.h"

void uart4_init(void);
void uart4_transmit_byte(char c);
void uart4_receive_byte(char *c);
void uart4_transmit_string (char s[]);

#endif /* INC_UART4_H_ */
