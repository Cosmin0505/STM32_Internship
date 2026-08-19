#include "main.h"
#include "adc1.h"
#include "lcd.h"
#include "stm32f3xx.h"
#include "led_gpio.h"
#include "sys_clock.h"
#include <stdio.h>

#define KPH_CONSTANT 0.0135648

uint32_t conv = 0;
uint32_t speed = 0;
char rpm[15];
char kph[15];

int main(void)
{
	clock_init();
	gpiod_init();
	adc1_init();
	i2c1_master_init();
	lcd_init();


	lcd_sendcommand(0x80);
	lcd_write("RPM: ");
	lcd_sendcommand(0xC0);
	lcd_write("Speed: ");
	while(1)
	{
		adc1_start_conversion();
		conv = adc1_read();
		speed = KPH_CONSTANT * conv;
		if(conv < 1365)
		{
			green_led(0U);
			yellow_led(0U);
			red_led(0U);
		}
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

		sprintf(rpm, "%d    ", conv);
		sprintf(kph, "%d    ", speed);
		lcd_sendcommand(0x85);
		lcd_write(rpm);
		lcd_sendcommand(0xC7);
		lcd_write(kph);
	}

}


