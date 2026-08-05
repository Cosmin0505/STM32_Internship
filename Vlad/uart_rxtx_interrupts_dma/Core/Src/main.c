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
	dma1_init();
	dma2_init();

	uart4_write("\nSalut Cosmin!\n\r");

	while(1)
	{
		__WFI();
	}

}

