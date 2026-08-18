#include "main.h"
#include "adc1.h"
#include "stm32f3xx.h"
#include "led_gpio.h"
#include "sys_clock.h"

uint32_t conv = 0;

int main(void)
{
	clock_init();
	gpiod_init();
	adc1_init();

	while(1)
	{
		adc1_start_conversion();
		conv = adc1_read();
		if(conv >= 1365 && conv < 2730)
		{
			green_led(1U);
			yellow_led(0U);
			red_led(0U);
		}
		if(conv >= 2730 && conv < 3500)
		{
			green_led(1U);
			yellow_led(1U);
			red_led(0U);
		}
		if(conv >= 3500)
		{
			green_led(1U);
			yellow_led(1U);
			red_led(1U);
		}
	}

}
