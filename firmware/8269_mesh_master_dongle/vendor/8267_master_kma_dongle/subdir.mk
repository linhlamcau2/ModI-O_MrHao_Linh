################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../vendor/8267_master_kma_dongle/app.c \
../vendor/8267_master_kma_dongle/app_att.c \
../vendor/8267_master_kma_dongle/app_ota.c \
../vendor/8267_master_kma_dongle/app_pair.c \
../vendor/8267_master_kma_dongle/audio_buffer.c \
../vendor/8267_master_kma_dongle/button.c \
../vendor/8267_master_kma_dongle/e2prom.c \
../vendor/8267_master_kma_dongle/main.c \
../vendor/8267_master_kma_dongle/tl_audio.c 

OBJS += \
./vendor/8267_master_kma_dongle/app.o \
./vendor/8267_master_kma_dongle/app_att.o \
./vendor/8267_master_kma_dongle/app_ota.o \
./vendor/8267_master_kma_dongle/app_pair.o \
./vendor/8267_master_kma_dongle/audio_buffer.o \
./vendor/8267_master_kma_dongle/button.o \
./vendor/8267_master_kma_dongle/e2prom.o \
./vendor/8267_master_kma_dongle/main.o \
./vendor/8267_master_kma_dongle/tl_audio.o 


# Each subdirectory must supply rules for building sources it contributes
vendor/8267_master_kma_dongle/%.o: ../vendor/8267_master_kma_dongle/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 Compiler'
	tc32-elf-gcc -ffunction-sections -fdata-sections -I"D:\Code\BLE\Test\RD_Switch_Touch_V3.1\RD_Switch_Touch_V3.1\firmware" -D__PROJECT_8267_MASTER_KMA_DONGLE__=1 -D__telink__ -DCHIP_TYPE=CHIP_TYPE_8269 -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


