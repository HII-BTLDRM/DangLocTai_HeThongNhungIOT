################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/PC/Downloads/SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/STM32F401RE_StdPeriph_Driver/src/stm32f401re_rcc.c 

OBJS += \
./SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/STM32F401RE_StdPeriph_Driver/src/stm32f401re_rcc.o 

C_DEPS += \
./SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/STM32F401RE_StdPeriph_Driver/src/stm32f401re_rcc.d 


# Each subdirectory must supply rules for building sources it contributes
SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/STM32F401RE_StdPeriph_Driver/src/stm32f401re_rcc.o: C:/Users/PC/Downloads/SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/STM32F401RE_StdPeriph_Driver/src/stm32f401re_rcc.c SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/STM32F401RE_StdPeriph_Driver/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNUCLEO_F401RE -DSTM32 -DSTM32F401RETx -DSTM32F4 -c -I../Inc -I"C:/Users/PC/Downloads/SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/CMSIS/Include" -I"C:/Users/PC/Desktop/New folder/DangLocTai-HeThongNhungIOT/Bai8-CauHinhNgatChoChanGPIO" -I"C:/Users/PC/Downloads/SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/CMSIS/Include" -I"C:/Users/PC/Downloads/SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/STM32F401RE_StdPeriph_Driver/inc" -I"C:/Users/PC/Downloads/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/button" -I"C:/Users/PC/Downloads/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/buzzer" -I"C:/Users/PC/Downloads/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/led" -I"C:/Users/PC/Downloads/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/rtos" -I"C:/Users/PC/Downloads/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/sensor" -I"C:/Users/PC/Downloads/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/serial" -I"C:/Users/PC/Downloads/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/ucglib" -I"C:/Users/PC/Downloads/SDK_1.0.3_NUCLEO-F401RE/shared/Utilities" -I"C:/Users/PC/Downloads/SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/STM32F401RE_StdPeriph_Driver/inc" -I"C:/Users/PC/Downloads/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/button" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

