/*
 * FreeRTOSConfig.h
 *
 * Target: STM32F303xC
 * Core: Cortex-M4F
 * Compiler: GCC
 * Kernel: FreeRTOS V11.3.1
 *
 * FreeRTOS directly owns SVC, PendSV and SysTick.
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdint.h>
#include <stddef.h>
#include "stm32f3xx.h"

/*-----------------------------------------------------------
 * Scheduler configuration
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION                         1
#define configUSE_TIME_SLICING                       1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION      1
#define configUSE_TICKLESS_IDLE                      0

/*
 * SystemCoreClock must contain the real processor frequency before
 * vTaskStartScheduler() is called.
 */
#define configCPU_CLOCK_HZ                           ( SystemCoreClock )

/*
 * SysTick is clocked from the processor clock, so
 * configSYSTICK_CLOCK_HZ is intentionally not defined.
 */
#define configTICK_RATE_HZ                           ( ( TickType_t ) 1000U )
#define configTICK_TYPE_WIDTH_IN_BITS                TICK_TYPE_WIDTH_32_BITS

#define configMAX_PRIORITIES                         5
#define configMINIMAL_STACK_SIZE                     128U
#define configMAX_TASK_NAME_LEN                      16
#define configIDLE_SHOULD_YIELD                      1

/*
 * ARM_CM4F optimized task selection supports no more than
 * 32 task priorities.
 */
#if ( configMAX_PRIORITIES > 32 )
    #error configMAX_PRIORITIES_must_not_exceed_32
#endif

/*-----------------------------------------------------------
 * Task features
 *----------------------------------------------------------*/

#define configUSE_TASK_NOTIFICATIONS                 1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES        1

#define configUSE_MUTEXES                            1
#define configUSE_RECURSIVE_MUTEXES                  1
#define configUSE_COUNTING_SEMAPHORES                1
#define configUSE_QUEUE_SETS                         0

#define configQUEUE_REGISTRY_SIZE                    8
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS      0
#define configUSE_APPLICATION_TASK_TAG               0

#define configENABLE_BACKWARD_COMPATIBILITY          1
#define configUSE_NEWLIB_REENTRANT                   0
#define configUSE_POSIX_ERRNO                        0

/*-----------------------------------------------------------
 * Event groups and stream buffers
 *----------------------------------------------------------*/

#define configUSE_EVENT_GROUPS                       1
#define configUSE_STREAM_BUFFERS                     1
#define configUSE_SB_COMPLETED_CALLBACK              0

/*-----------------------------------------------------------
 * Software timers
 *----------------------------------------------------------*/

#define configUSE_TIMERS                             1
#define configTIMER_TASK_PRIORITY                    \
    ( configMAX_PRIORITIES - 1 )

/*
 * Stack sizes are expressed in StackType_t units.
 * On Cortex-M4, 256 words normally means 1024 bytes.
 */
#define configTIMER_TASK_STACK_DEPTH                 256U
#define configTIMER_QUEUE_LENGTH                     10

/*-----------------------------------------------------------
 * Memory allocation
 *----------------------------------------------------------*/

/*
 * This configuration uses heap_4.c.
 */
#define configSUPPORT_STATIC_ALLOCATION              0
#define configSUPPORT_DYNAMIC_ALLOCATION             1

/*
 * 12 KiB is sufficient for the initial Blink task, Idle task,
 * Timer task and basic kernel objects.
 */
#define configTOTAL_HEAP_SIZE                        \
    ( ( size_t ) ( 12U * 1024U ) )

#define configAPPLICATION_ALLOCATED_HEAP             0
#define configSTACK_ALLOCATION_FROM_SEPARATE_HEAP    0
#define configHEAP_CLEAR_MEMORY_ON_FREE              0
#define configENABLE_HEAP_PROTECTOR                  0

/*-----------------------------------------------------------
 * Hook and diagnostic configuration
 *----------------------------------------------------------*/

#define configUSE_IDLE_HOOK                          0
#define configUSE_TICK_HOOK                          0
#define configUSE_MALLOC_FAILED_HOOK                 1
#define configUSE_DAEMON_TASK_STARTUP_HOOK           0

#define configCHECK_FOR_STACK_OVERFLOW               2

#define configGENERATE_RUN_TIME_STATS                0
#define configUSE_TRACE_FACILITY                     1
#define configUSE_STATS_FORMATTING_FUNCTIONS         0

/*-----------------------------------------------------------
 * Co-routines
 *----------------------------------------------------------*/

#define configUSE_CO_ROUTINES                        0
#define configMAX_CO_ROUTINE_PRIORITIES              1

/*-----------------------------------------------------------
 * Cortex-M4 NVIC interrupt priorities
 *----------------------------------------------------------*/

/*
 * STM32F303 implements four NVIC priority bits.
 */
#define configPRIO_BITS                              __NVIC_PRIO_BITS

#if ( __NVIC_PRIO_BITS != 4U )
    #error Unexpected_NVIC_priority_bit_count
#endif

/*
 * Logical STM32 priorities:
 *
 * 0  = highest urgency
 * 15 = lowest urgency
 *
 * ISRs with logical priorities 0-4 must not call FreeRTOS APIs.
 * ISRs with logical priorities 5-15 may call FromISR APIs.
 */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY      15U
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5U

/*
 * Raw NVIC register encodings:
 *
 * Kernel priority:      15 << 4 = 0xF0
 * Max syscall priority:  5 << 4 = 0x50
 */
#define configKERNEL_INTERRUPT_PRIORITY              \
    ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY <<     \
      ( 8U - configPRIO_BITS ) )

#define configMAX_SYSCALL_INTERRUPT_PRIORITY         \
    ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << \
      ( 8U - configPRIO_BITS ) )

/*
 * Verify that the vector table points directly to the FreeRTOS
 * exception handlers.
 */
#define configCHECK_HANDLER_INSTALLATION             1

/*-----------------------------------------------------------
 * Cortex-M exception-handler mappings
 *----------------------------------------------------------*/

/*
 * These definitions cause ARM_CM4F/port.c to provide the actual
 * SVC_Handler, PendSV_Handler and SysTick_Handler symbols.
 *
 * Do not define these handlers in stm32f3xx_it.c.
 */
#define vPortSVCHandler                              SVC_Handler
#define xPortPendSVHandler                           PendSV_Handler
#define xPortSysTickHandler                          SysTick_Handler

/*-----------------------------------------------------------
 * Optional API functions
 *----------------------------------------------------------*/

#define INCLUDE_vTaskPrioritySet                     1
#define INCLUDE_uxTaskPriorityGet                    1
#define INCLUDE_vTaskDelete                         1
#define INCLUDE_vTaskSuspend                        1
#define INCLUDE_xTaskDelayUntil                     1
#define INCLUDE_vTaskDelay                          1
#define INCLUDE_xTaskGetSchedulerState               1
#define INCLUDE_xTaskGetCurrentTaskHandle            1
#define INCLUDE_uxTaskGetStackHighWaterMark          1
#define INCLUDE_xTaskGetIdleTaskHandle               1
#define INCLUDE_eTaskGetState                        1
#define INCLUDE_xTimerPendFunctionCall               1
#define INCLUDE_xTaskAbortDelay                      1
#define INCLUDE_xTaskGetHandle                       1
#define INCLUDE_xTaskResumeFromISR                   1

/*-----------------------------------------------------------
 * Assertions
 *----------------------------------------------------------*/

#define configASSERT( expression )                   \
    do                                               \
    {                                                \
        if( ( expression ) == 0 )                    \
        {                                            \
            __disable_irq();                         \
                                                     \
            for( ;; )                                \
            {                                        \
                /* Stop here for debugger inspection. */ \
            }                                        \
        }                                            \
    } while( 0 )

#endif /* FREERTOS_CONFIG_H */
