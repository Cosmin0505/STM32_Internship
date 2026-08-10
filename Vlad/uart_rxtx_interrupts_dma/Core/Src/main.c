#include "main.h"
#include "stm32f3xx.h"

#include "usart1.h"
#include "uart4.h"
#include "sys_clock.h"
#include "stdbool.h"

char c_in_uart4 = ' ';
char c_in_uart1 = ' ';

char pc_received_string[MAX_LENGTH];
char string_to_be_transmitted[MAX_LENGTH] = "Vlad: ";

char received_string_uart4[MAX_LENGTH];
char string_to_be_transmitted_uart4[MAX_LENGTH] = "Cosmin: ";

int main(void)
{
	clock_init();
	usart1_init();
	uart4_init();
	dma1_init();
	dma2_init();

	while(1)
	{
		__WFI();
	}

}

