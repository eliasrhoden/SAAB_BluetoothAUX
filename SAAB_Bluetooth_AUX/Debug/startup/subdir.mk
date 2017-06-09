################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../startup/startup_stm32f303x8.s 

OBJS += \
./startup/startup_stm32f303x8.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo %cd%
	arm-none-eabi-as -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -I"C:/Users/elias/git/SAAB_Bluetooth/SAAB_Bluetooth_AUX/inc" -I"C:/Users/elias/git/SAAB_Bluetooth/SAAB_Bluetooth_AUX/CMSIS/core" -I"C:/Users/elias/git/SAAB_Bluetooth/SAAB_Bluetooth_AUX/CMSIS/device" -I"C:/Users/elias/git/SAAB_Bluetooth/SAAB_Bluetooth_AUX/HAL_Driver/Inc/Legacy" -I"C:/Users/elias/git/SAAB_Bluetooth/SAAB_Bluetooth_AUX/HAL_Driver/Inc" -I"C:/Users/elias/git/SAAB_Bluetooth/SAAB_Bluetooth_AUX/Utilities/STM32F3xx_Nucleo_32" -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


