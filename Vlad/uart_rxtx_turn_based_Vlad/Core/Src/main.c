#include "main.h"
#include "stm32f3xx.h"

#include "usart1.h"
#include "uart4.h"
#include "sys_clock.h"
#include "stdbool.h"

int main(void)
{
	clock_init();
	usart1_init();
	uart4_init();

//	uart4_transmit_byte('b');

	while(1)
	{
		__WFI();
	}

}

