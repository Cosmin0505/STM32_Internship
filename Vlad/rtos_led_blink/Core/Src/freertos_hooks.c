#include "stm32f3xx.h"
#include "FreeRTOS.h"
#include "task.h"

void vApplicationMallocFailedHook(void)
{
    __disable_irq();

    for (;;)
    {
        /* Set a debugger breakpoint here. */
    }
}

void vApplicationStackOverflowHook(
    TaskHandle_t task,
    char *taskName)
{
    (void)task;
    (void)taskName;

    __disable_irq();

    for (;;)
    {
        /* Inspect taskName in the debugger. */
    }
}
