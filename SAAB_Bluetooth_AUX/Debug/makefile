################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include startup/subdir.mk
-include src/RN52/subdir.mk
-include src/Debug/subdir.mk
-include src/CAN/subdir.mk
-include src/subdir.mk
-include Utilities/STM32F3xx_Nucleo_32/subdir.mk
-include HAL_Driver/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: SAAB_Bluetooth_AUX.elf

# Tool invocations
SAAB_Bluetooth_AUX.elf: $(OBJS) $(USER_OBJS) C:\Users\elias\git\SAAB_Bluetooth\SAAB_Bluetooth_AUX\LinkerScript.ld
	@echo 'Building target: $@'
	@echo 'Invoking: MCU GCC Linker'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -T"C:\Users\elias\git\SAAB_Bluetooth\SAAB_Bluetooth_AUX\LinkerScript.ld" -Wl,-Map=output.map -Wl,--gc-sections -lm -o "SAAB_Bluetooth_AUX.elf" @"objects.list" $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

post-build:
	-@echo 'Generating binary and Printing size information:'
	arm-none-eabi-objcopy -O binary "SAAB_Bluetooth_AUX.elf" "SAAB_Bluetooth_AUX.bin"
	arm-none-eabi-size "SAAB_Bluetooth_AUX.elf"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
