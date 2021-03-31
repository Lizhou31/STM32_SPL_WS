# Target Name
TARGET = testopenocd

# Gcc none eabi define
CC = arm-none-eabi-gcc
CP = arm-none-eabi-objcopy

# Flash 
ST_FLASH ?= st-flash

# Debug Flag
DEBUG = 1

# Optimize option
OPT = -Og

# Build Directory
BUILD_DIR = $(PRJ_ROOT)/build

# WorkSpace Definition
PRJ_ROOT ?= ../..
STD_PERIPH_LIBS ?= $(PRJ_ROOT)/libraries/STM32F10x_StdPeriph_Lib_V3.5.0
TARGET_ROOT ?= $(PRJ_ROOT)/src/$(TARGET)
MY_LIBS ?= $(PRJ_ROOT)/libraries/MyLIBS
LD_FILE ?= $(PRJ_ROOT)/conf

# C_Sources 
C_SOURCES  = $(TARGET_ROOT)/src/*.c
C_SOURCES += $(STD_PERIPH_LIBS)/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c
#C_SOURCES += $(STD_PERIPH_LIBS)/Libraries/STM32F10x_StdPeriph_Driver/src/*.c
C_SOURCES += $(STD_PERIPH_LIBS)/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/TrueSTUDIO/startup_stm32f10x_md.s
#C_SOURCES += $(MY_LIBS)/*.c

# Arm CPU Define
CPU = -mcpu=cortex-m3
MCU = $(CPU) -mthumb

# SPL Define
C_DEFS = -DSTM32F10X_MD \
		\-DUSE_STDPERIPH_DRIVER

# Include Lib
C_INCLUDES = -I$(TARGET_ROOT)/inc \
	     -I$(STD_PERIPH_LIBS)/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/ \
	     -I$(STD_PERIPH_LIBS)/Libraries/CMSIS/CM3/CoreSupport \
#	     -I$(STD_PERIPH_LIBS)/Libraries/STM32F10x_StdPeriph_Driver/inc \
		 -I$(MY_LIBS)/

# C Flag
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections 

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

# Link Flag
LIBS = -lc -lm -lrdimon -lnosys
LDSCRIPT = $(LD_FILE)/STM32F103RCTx_FLASH.ld
LDFLAGS = $(MCU) -specs=nano.specs -specs=rdimon.specs -specs=nosys.specs -T$(LDSCRIPT) $(LIBS)

all: $(BUILD_DIR)/$(TARGET).elf

$(shell   mkdir -p $(BUILD_DIR))

# compile
$(BUILD_DIR)/$(TARGET).elf: $(C_SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
	$(CP) -O ihex $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex
	$(CP) -O binary $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin

clean:
	rm -rf $(BUILD_DIR)

upload:
	sudo $(ST_FLASH) write $(BUILD_DIR)/$(TARGET).bin 0x8000000


