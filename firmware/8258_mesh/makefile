################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include vendor/user_app/subdir.mk
-include vendor/mesh/subdir.mk
-include vendor/mesh/RD_Switch/subdir.mk
-include vendor/mesh/RD_Secure/subdir.mk
-include vendor/mesh/RD_MessData/subdir.mk
-include vendor/mesh/RD_Flash/subdir.mk
-include vendor/mesh/LC8823_LED/subdir.mk
-include vendor/common/subdir.mk
-include vendor/common/nl_api/subdir.mk
-include vendor/common/mi_api/subdir.mk
-include vendor/common/mi_api/mijia_ble_api/subdir.mk
-include vendor/common/mi_api/mi_vendor/subdir.mk
-include vendor/common/mi_api/libs/third_party/micro-ecc/subdir.mk
-include vendor/common/mi_api/libs/subdir.mk
-include proj/mcu_spec/subdir.mk
-include proj/mcu/subdir.mk
-include proj/drivers/subdir.mk
-include proj/common/subdir.mk
-include proj/app/subdir.mk
-include drivers/8258/subdir.mk
-include drivers/8258/flash/subdir.mk
-include boot/8258/subdir.mk
-include objects.mk

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LST += \
8258_mesh.lst \

FLASH_IMAGE += \

SIZEDUMMY += \
sizedummy \


# All Target
all: pre-build main-build

# Main-build Target
main-build: RD_Touch_Switch_V3.1.elf secondary-outputs

# Tool invocations
RD_Touch_Switch_V3.1.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: TC32 C Linker'
	tc32-elf-ld --gc-sections -L"C:\Users\NV-Smart03\Downloads\RD_Switch_Touch_V3.1-main\RD_Switch_Touch_V3.1-main\firmware\proj_lib" -T ../boot.link -o"RD_Touch_Switch_V3.1.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

8258_mesh.lst: RD_Touch_Switch_V3.1.elf
	@echo 'Invoking: TC32 Create Extended Listing'
	tc32-elf-objdump -x -D -l -S RD_Touch_Switch_V3.1.elf  >"8258_mesh.lst"
	@echo 'Finished building: $@'
	@echo ' '

: RD_Touch_Switch_V3.1.elf
	@echo 'Create Flash image (binary format)'
	tc32-elf-objcopy -O binary RD_Touch_Switch_V3.1.elf
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: RD_Touch_Switch_V3.1.elf
	@echo 'Invoking: Print Size'
	tc32-elf-size -t RD_Touch_Switch_V3.1.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(LST)$(SIZEDUMMY) RD_Touch_Switch_V3.1.elf
	-@echo ' '

pre-build:
	-"C:\Users\NV-Smart03\Downloads\RD_Switch_Touch_V3.1-main\RD_Switch_Touch_V3.1-main\firmware/getver.sh"
	-@echo ' '

post-build:
	-"C:\Users\NV-Smart03\Downloads\RD_Switch_Touch_V3.1-main\RD_Switch_Touch_V3.1-main\firmware/tl_check_fw.sh"   8258_mesh   RD_Touch_Switch_V3.1
	-@echo ' '

secondary-outputs: $(LST) $(FLASH_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY: main-build pre-build post-build

-include ../makefile.targets
