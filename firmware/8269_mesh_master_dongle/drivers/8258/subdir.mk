################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/8258/flash.c \
../drivers/8258/flash_mesh_extend.c 

OBJS += \
./drivers/8258/flash.o \
./drivers/8258/flash_mesh_extend.o 


# Each subdirectory must supply rules for building sources it contributes
drivers/8258/%.o: ../drivers/8258/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 Compiler'
	tc32-elf-gcc -ffunction-sections -fdata-sections -I"D:\Code\BLE\Test\RD_Switch_Touch_V3.1\RD_Switch_Touch_V3.1\firmware" -D__PROJECT_8267_MASTER_KMA_DONGLE__=1 -D__telink__ -DCHIP_TYPE=CHIP_TYPE_8269 -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


