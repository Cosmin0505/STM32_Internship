################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middleware/ThirdParty/FreeRTOS/croutine.c \
../Middleware/ThirdParty/FreeRTOS/event_groups.c \
../Middleware/ThirdParty/FreeRTOS/list.c \
../Middleware/ThirdParty/FreeRTOS/queue.c \
../Middleware/ThirdParty/FreeRTOS/stream_buffer.c \
../Middleware/ThirdParty/FreeRTOS/tasks.c \
../Middleware/ThirdParty/FreeRTOS/timers.c 

OBJS += \
./Middleware/ThirdParty/FreeRTOS/croutine.o \
./Middleware/ThirdParty/FreeRTOS/event_groups.o \
./Middleware/ThirdParty/FreeRTOS/list.o \
./Middleware/ThirdParty/FreeRTOS/queue.o \
./Middleware/ThirdParty/FreeRTOS/stream_buffer.o \
./Middleware/ThirdParty/FreeRTOS/tasks.o \
./Middleware/ThirdParty/FreeRTOS/timers.o 

C_DEPS += \
./Middleware/ThirdParty/FreeRTOS/croutine.d \
./Middleware/ThirdParty/FreeRTOS/event_groups.d \
./Middleware/ThirdParty/FreeRTOS/list.d \
./Middleware/ThirdParty/FreeRTOS/queue.d \
./Middleware/ThirdParty/FreeRTOS/stream_buffer.d \
./Middleware/ThirdParty/FreeRTOS/tasks.d \
./Middleware/ThirdParty/FreeRTOS/timers.d 


# Each subdirectory must supply rules for building sources it contributes
Middleware/ThirdParty/FreeRTOS/%.o Middleware/ThirdParty/FreeRTOS/%.su: ../Middleware/ThirdParty/FreeRTOS/%.c Middleware/ThirdParty/FreeRTOS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303xC -c -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/savav/stm32f3_rtos/rtos_hello_world/Middleware/ThirdParty/FreeRTOS/include" -I"C:/Users/savav/stm32f3_rtos/rtos_hello_world/Middleware/ThirdParty/FreeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/savav/stm32f3_rtos/rtos_hello_world/Middleware/ThirdParty/FreeRTOS" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middleware-2f-ThirdParty-2f-FreeRTOS

clean-Middleware-2f-ThirdParty-2f-FreeRTOS:
	-$(RM) ./Middleware/ThirdParty/FreeRTOS/croutine.d ./Middleware/ThirdParty/FreeRTOS/croutine.o ./Middleware/ThirdParty/FreeRTOS/croutine.su ./Middleware/ThirdParty/FreeRTOS/event_groups.d ./Middleware/ThirdParty/FreeRTOS/event_groups.o ./Middleware/ThirdParty/FreeRTOS/event_groups.su ./Middleware/ThirdParty/FreeRTOS/list.d ./Middleware/ThirdParty/FreeRTOS/list.o ./Middleware/ThirdParty/FreeRTOS/list.su ./Middleware/ThirdParty/FreeRTOS/queue.d ./Middleware/ThirdParty/FreeRTOS/queue.o ./Middleware/ThirdParty/FreeRTOS/queue.su ./Middleware/ThirdParty/FreeRTOS/stream_buffer.d ./Middleware/ThirdParty/FreeRTOS/stream_buffer.o ./Middleware/ThirdParty/FreeRTOS/stream_buffer.su ./Middleware/ThirdParty/FreeRTOS/tasks.d ./Middleware/ThirdParty/FreeRTOS/tasks.o ./Middleware/ThirdParty/FreeRTOS/tasks.su ./Middleware/ThirdParty/FreeRTOS/timers.d ./Middleware/ThirdParty/FreeRTOS/timers.o ./Middleware/ThirdParty/FreeRTOS/timers.su

.PHONY: clean-Middleware-2f-ThirdParty-2f-FreeRTOS

