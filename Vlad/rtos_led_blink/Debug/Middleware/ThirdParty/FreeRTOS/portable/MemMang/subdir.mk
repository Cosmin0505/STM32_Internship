################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middleware/ThirdParty/FreeRTOS/portable/MemMang/heap_4.c 

OBJS += \
./Middleware/ThirdParty/FreeRTOS/portable/MemMang/heap_4.o 

C_DEPS += \
./Middleware/ThirdParty/FreeRTOS/portable/MemMang/heap_4.d 


# Each subdirectory must supply rules for building sources it contributes
Middleware/ThirdParty/FreeRTOS/portable/MemMang/%.o Middleware/ThirdParty/FreeRTOS/portable/MemMang/%.su: ../Middleware/ThirdParty/FreeRTOS/portable/MemMang/%.c Middleware/ThirdParty/FreeRTOS/portable/MemMang/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303xC -c -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/savav/stm32f3_rtos/rtos_hello_world/Middleware/ThirdParty/FreeRTOS/include" -I"C:/Users/savav/stm32f3_rtos/rtos_hello_world/Middleware/ThirdParty/FreeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/savav/stm32f3_rtos/rtos_hello_world/Middleware/ThirdParty/FreeRTOS" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middleware-2f-ThirdParty-2f-FreeRTOS-2f-portable-2f-MemMang

clean-Middleware-2f-ThirdParty-2f-FreeRTOS-2f-portable-2f-MemMang:
	-$(RM) ./Middleware/ThirdParty/FreeRTOS/portable/MemMang/heap_4.d ./Middleware/ThirdParty/FreeRTOS/portable/MemMang/heap_4.o ./Middleware/ThirdParty/FreeRTOS/portable/MemMang/heap_4.su

.PHONY: clean-Middleware-2f-ThirdParty-2f-FreeRTOS-2f-portable-2f-MemMang

