################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := uart_rxtx_interrupts
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
uart_rxtx_interrupts.elf \

MAP_FILES += \
uart_rxtx_interrupts.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
uart_rxtx_interrupts.list \


# All Target
all: main-build

# Main-build Target
main-build: uart_rxtx_interrupts.elf secondary-outputs

# Tool invocations
uart_rxtx_interrupts.elf uart_rxtx_interrupts.map: $(OBJS) $(USER_OBJS) C:\Users\busilam\Cosmin\Traingings\STM32_Bare_Metal\uart_rxtx_interrupts\STM32F070RBTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "uart_rxtx_interrupts.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m0 -T"C:\Users\busilam\Cosmin\Traingings\STM32_Bare_Metal\uart_rxtx_interrupts\STM32F070RBTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="uart_rxtx_interrupts.map" -Wl,--gc-sections -static -L"C:\Users\busilam\Cosmin\Traingings\STM32_Bare_Metal\STM32Cube_FW_F0_V1.10.0\Drivers\CMSIS\Include" -L"C:\Users\busilam\Cosmin\Traingings\STM32_Bare_Metal\STM32Cube_FW_F0_V1.10.0\Drivers\CMSIS\Device\ST\STM32F0xx\Include" --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

uart_rxtx_interrupts.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "uart_rxtx_interrupts.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) default.size.stdout uart_rxtx_interrupts.elf uart_rxtx_interrupts.list uart_rxtx_interrupts.map
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
