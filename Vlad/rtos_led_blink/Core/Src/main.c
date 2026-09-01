#include "stm32f3xx.h"
#include "FreeRTOS.h"
#include "task.h"

static void led1_init(void);
static void led2_init(void);
void BlinkTask1(void *argument);
void BlinkTask2(void *argument);

int main(void)
{
	led1_init();
	led2_init();

	xTaskCreate(BlinkTask1, "Blink1", 128, NULL, 1, NULL);
	xTaskCreate(BlinkTask2, "Blink2", 128, NULL, 2, NULL);

	vTaskStartScheduler();
	while(1)
	{

	}
	return 0;
}

static void led1_init(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;

	GPIOE->MODER &= ~(3U << (10 * 2));
	GPIOE->MODER |= (1U << (10 * 2));
}

static void led2_init(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;

	GPIOE->MODER &= ~(3U << (11 * 2));
	GPIOE->MODER |= (1U << (11 * 2));
}

void BlinkTask1(void *argument)
{
	for(;;)
	{
		GPIOE->ODR ^= (1U << 10);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void BlinkTask2(void *argument)
{
	for(;;)
	{
		GPIOE->ODR ^= (1U << 11);
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

