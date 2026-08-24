#include "stm32f3xx.h"
#include "main.h"
#include "sys_clock.h"
#include "adc1.h"
#include "lcd.h"
#include "led_gpio.h"
#include "pwm.h"
#include <stdio.h>

#define PROPELLER_DIAMETER 60U

#define PWM_MAX_COMPARE 1000U
#define ADC_SAMPLES 64U
#define ADC_MAX_COUNTS 4095U

/* DC motor parameters */
#define MOTOR_START_DUTY_CYCLE 195U
#define START_RPM 1200U
#define FULL_DUTY_RPM 12000U

static uint32_t estimate_target_rpm(uint32_t duty);
static uint32_t rpm_to_speed(uint32_t rpm);

uint32_t conv = 0;
uint32_t speed = 0;
uint32_t duty_cycle = 0;
uint32_t estimated_rpm = 0;

char rpm[16] = {0};
char kph[16] = {0};

int main(void)
{
	/* Initialize peripherals */
	clock_init();
	gpiod_init();
	adc1_init();
	i2c1_master_init();
	lcd_init();
	tim3_init();

	/* Print static text */
	lcd_sendcommand(0x80);
	lcd_write("RPM: ");
	lcd_sendcommand(0xC0);
	lcd_write("Speed: ");

	while(1)
	{
		/* Compute an average value to reduce noise */
		uint32_t sum = 0U;
		for (uint32_t i = 0; i < ADC_SAMPLES; i++) {
			adc1_start_conversion();
			sum += adc1_read();
		}
		conv = sum / ADC_SAMPLES;

		/* Convert ADC value to PWM duty cycle */
		duty_cycle = (uint32_t)(((uint64_t)conv * PWM_MAX_COMPARE
					 + ADC_MAX_COUNTS / 2U)
					 / ADC_MAX_COUNTS);

		tim3_update_duty_cycle(duty_cycle);

		estimated_rpm = estimate_target_rpm(duty_cycle);

		speed = rpm_to_speed(estimated_rpm);


		/* Indicate RPM ranges using LEDs */
		if(conv < 800)
		{
			green_led(0U);
			yellow_led(0U);
			red_led(0U);
		}
		else if(conv < 2000)
		{
			green_led(1U);
			yellow_led(0U);
			red_led(0U);
		}
		else if(conv < 3500)
		{
			green_led(1U);
			yellow_led(1U);
			red_led(0U);
		}
		else
		{
			green_led(1U);
			yellow_led(1U);
			red_led(1U);
		}

		/* Refresh LCD screen readings */
		sprintf(rpm, "%5u    ", (unsigned int)estimated_rpm);
		sprintf(kph, "%3u km/h ", (unsigned int)((speed +5U) / 10U));
		lcd_sendcommand(0x85);
		lcd_write(rpm);

		lcd_sendcommand(0xC7);
		lcd_write(kph);
	}

}

/* Linearly map PWM duty cycle to RPM */
static uint32_t estimate_target_rpm(uint32_t duty)
{
	if(duty < MOTOR_START_DUTY_CYCLE)
	{
		return 0;
	}

	uint32_t usable_duty = duty - MOTOR_START_DUTY_CYCLE;

	uint32_t usable_duty_range = 1000U - MOTOR_START_DUTY_CYCLE;

	uint32_t rpm_range = FULL_DUTY_RPM - START_RPM;

	return START_RPM + (uint32_t)(((uint64_t)rpm_range * usable_duty
			+ usable_duty_range / 2U) / usable_duty_range);
}

static uint32_t rpm_to_speed(uint32_t rpm)
{
	uint64_t numerator = (uint64_t)rpm * PROPELLER_DIAMETER
						* 188496ULL;

	return (uint32_t)((numerator + 50000000ULL) / 100000000ULL);
}
