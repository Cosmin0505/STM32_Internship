################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Inc/sys_clock.c \
../Core/Inc/uart4.c \
../Core/Inc/usart1.c 

OBJS += \
./Core/Inc/sys_clock.o \
./Core/Inc/uart4.o \
./Core/Inc/usart1.o 

C_DEPS += \
./Core/Inc/sys_clock.d \
./Core/Inc/uart4.d \
./Core/Inc/usart1.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Inc/%.o Core/Inc/%.su: ../Core/Inc/%.c Core/Inc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303xC -c -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Inc

clean-Core-2f-Inc:
	-$(RM) ./Core/Inc/sys_clock.d ./Core/Inc/sys_clock.o ./Core/Inc/sys_clock.su ./Core/Inc/uart4.d ./Core/Inc/uart4.o ./Core/Inc/uart4.su ./Core/Inc/usart1.d ./Core/Inc/usart1.o ./Core/Inc/usart1.su

.PHONY: clean-Core-2f-Inc

