################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../vendor/mesh/LC8823_LED/LC8823_Led.c 

OBJS += \
./vendor/mesh/LC8823_LED/LC8823_Led.o 


# Each subdirectory must supply rules for building sources it contributes
vendor/mesh/LC8823_LED/%.o: ../vendor/mesh/LC8823_LED/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 Compiler'
	tc32-elf-gcc -ffunction-sections -fdata-sections -I"C:\Users\NV-Smart03\Downloads\RD_Switch_Touch_V3.1-main\RD_Switch_Touch_V3.1-main\firmware" -I"C:\Users\NV-Smart03\Downloads\RD_Switch_Touch_V3.1-main\RD_Switch_Touch_V3.1-main\firmware\vendor\common\mi_api\libs" -I"C:\Users\NV-Smart03\Downloads\RD_Switch_Touch_V3.1-main\RD_Switch_Touch_V3.1-main\firmware\vendor\common\mi_api\mijia_ble_api" -D__PROJECT_MESH__=1 -D__telink__ -DCHIP_TYPE=CHIP_TYPE_8258 -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

