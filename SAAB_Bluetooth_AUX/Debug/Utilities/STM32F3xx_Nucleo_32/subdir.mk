################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Utilities/STM32F3xx_Nucleo_32/stm32f3xx_nucleo_32.c 

OBJS += \
./Utilities/STM32F3xx_Nucleo_32/stm32f3xx_nucleo_32.o 

C_DEPS += \
./Utilities/STM32F3xx_Nucleo_32/stm32f3xx_nucleo_32.d 


# Each subdirectory must supply rules for building sources it contributes
Utilities/STM32F3xx_Nucleo_32/%.o: ../Utilities/STM32F3xx_Nucleo_32/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -DSTM32F30 -DSTM32F303K8Tx -DSTM32F3 -DNUCLEO_F303K8 -DSTM32 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -I"C:/Users/elias/git/SAAB_Bluetooth/SAAB_Bluetooth_AUX/inc" -I"C:/Users/elias/git/SAAB_Bluetooth/SAAB_Bluetooth_AUX/CMSIS/core" -I"C:/Users/elias/git/SAAB_Bluetooth/SAAB_Bluetooth_AUX/CMSIS/device" -I"C:/Users/elias/git/SAAB_Bluetooth/SAAB_Bluetooth_AUX/HAL_Driver/Inc/Legacy" -I"C:/Users/elias/git/SAAB_Bluetooth/SAAB_Bluetooth_AUX/HAL_Driver/Inc" -I"C:/Users/elias/git/SAAB_Bluetooth/SAAB_Bluetooth_AUX/Utilities/STM32F3xx_Nucleo_32" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


