#include "stm32f3xx.h"
#include "main.h"
#include "sys_clock.h"
#include "adc1.h"
#include "lcd.h"
#include "led_gpio.h"
#include "pwm.h"
#include <stdio.h>

#define KPH_CONSTANT 0.0135648
#define ADC_DUTY_CYCLE_CONSTANT 0.2442

uint32_t conv = 0;
uint32_t speed = 0;
uint32_t apb1_frequency = 0;
uint32_t duty_cycle = 0;
int32_t increment = 100;
char rpm[15];
char kph[15];

int main(void)
{
	clock_init();
	gpiod_init();
	adc1_init();
	i2c1_master_init();
	lcd_init();
	tim3_init();


	lcd_sendcommand(0x80);
	lcd_write("RPM: ");
	lcd_sendcommand(0xC0);
	lcd_write("Speed: ");

	while(1)
	{
		/* Compute an average value to reduce noise */
		uint32_t sum = 0;
		for (uint32_t i = 0; i < 10 * 4000; i++) {
			adc1_start_conversion();
			sum += adc1_read();
		}
		conv = sum / 40000;
		duty_cycle = conv * ADC_DUTY_CYCLE_CONSTANT;
		tim3_update_duty_cycle(duty_cycle);
		speed = KPH_CONSTANT * conv;
		if(conv < 800)
		{
			green_led(0U);
			yellow_led(0U);
			red_led(0U);
		}
		if(conv >= 800 && conv < 2000)
		{
			green_led(1U);
			yellow_led(0U);
			red_led(0U);
		}
		if(conv >= 2000 && conv < 3500)
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


