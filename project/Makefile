######################################
# target
######################################
TARGET = environment-manager

.PHONY: all test clean

######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og


#######################################
# paths
#######################################
# Build path
BUILD_DIR = obj
BIN_DIR = bin

COMMON_PATH = ../Core/common
HAL_PATH = ../Drivers/STM32F1xx_HAL_Driver
FREERTOS_SRCDIR = ../Middlewares/Third_Party/FreeRTOS/Source

COMMON_SRCDIR = $(COMMON_PATH)/Src
HAL_SRCDIR = $(HAL_PATH)/Src

######################################
# source
######################################
# C sources
C_SOURCES =  \
$(COMMON_SRCDIR)/main.c \
$(COMMON_SRCDIR)/gpio.c \
$(COMMON_SRCDIR)/freertos.c \
$(COMMON_SRCDIR)/adc.c \
$(COMMON_SRCDIR)/crc.c \
$(COMMON_SRCDIR)/i2c.c \
$(COMMON_SRCDIR)/iwdg.c \
$(COMMON_SRCDIR)/rtc.c \
$(COMMON_SRCDIR)/spi.c \
$(COMMON_SRCDIR)/usart.c \
$(COMMON_SRCDIR)/wwdg.c \
$(COMMON_SRCDIR)/stm32f1xx_it.c \
$(COMMON_SRCDIR)/stm32f1xx_hal_msp.c \
$(COMMON_SRCDIR)/stm32f1xx_hal_timebase_tim.c \
$(COMMON_SRCDIR)/system_stm32f1xx.c \
$(HAL_SRCDIR)/stm32f1xx_hal_gpio_ex.c \
$(HAL_SRCDIR)/stm32f1xx_hal.c \
$(HAL_SRCDIR)/stm32f1xx_hal_rcc.c \
$(HAL_SRCDIR)/stm32f1xx_hal_rcc_ex.c \
$(HAL_SRCDIR)/stm32f1xx_hal_gpio.c \
$(HAL_SRCDIR)/stm32f1xx_hal_dma.c \
$(HAL_SRCDIR)/stm32f1xx_hal_cortex.c \
$(HAL_SRCDIR)/stm32f1xx_hal_pwr.c \
$(HAL_SRCDIR)/stm32f1xx_hal_flash.c \
$(HAL_SRCDIR)/stm32f1xx_hal_flash_ex.c \
$(HAL_SRCDIR)/stm32f1xx_hal_exti.c \
$(HAL_SRCDIR)/stm32f1xx_hal_adc.c \
$(HAL_SRCDIR)/stm32f1xx_hal_adc_ex.c \
$(HAL_SRCDIR)/stm32f1xx_hal_crc.c \
$(HAL_SRCDIR)/stm32f1xx_hal_i2c.c \
$(HAL_SRCDIR)/stm32f1xx_hal_iwdg.c \
$(HAL_SRCDIR)/stm32f1xx_hal_rtc.c \
$(HAL_SRCDIR)/stm32f1xx_hal_rtc_ex.c \
$(HAL_SRCDIR)/stm32f1xx_hal_spi.c \
$(HAL_SRCDIR)/stm32f1xx_hal_tim.c \
$(HAL_SRCDIR)/stm32f1xx_hal_tim_ex.c \
$(HAL_SRCDIR)/stm32f1xx_hal_uart.c \
$(HAL_SRCDIR)/stm32f1xx_hal_wwdg.c \
$(FREERTOS_SRCDIR)/croutine.c \
$(FREERTOS_SRCDIR)/event_groups.c \
$(FREERTOS_SRCDIR)/list.c \
$(FREERTOS_SRCDIR)/queue.c \
$(FREERTOS_SRCDIR)/stream_buffer.c \
$(FREERTOS_SRCDIR)/tasks.c \
$(FREERTOS_SRCDIR)/timers.c \
$(FREERTOS_SRCDIR)/CMSIS_RTOS_V2/cmsis_os2.c \
$(FREERTOS_SRCDIR)/portable/MemMang/heap_4.c \
$(FREERTOS_SRCDIR)/portable/GCC/ARM_CM3/port.c

# ASM sources
ASM_SOURCES =  \
startup_stm32f103xb.s


#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m3

# fpu
# NONE for Cortex-M0/M0+/M3

# float-abi


# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32F103xB


# AS includes
AS_INCLUDES =  \
-I../Core/common/Inc

# C includes
C_INCLUDES =  \
-I$(COMMON_PATH)/Inc \
-I$(HAL_PATH)/Inc \
-I$(HAL_PATH)/Inc/Legacy \
-I$(FREERTOS_SRCDIR)/include \
-I$(FREERTOS_SRCDIR)/CMSIS_RTOS_V2 \
-I$(FREERTOS_SRCDIR)/portable/GCC/ARM_CM3 \
-I../Drivers/CMSIS/Device/ST/STM32F1xx/Include \
-I../Drivers/CMSIS/Include


# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) 
ASFLAGS += -Wall 
ASFLAGS += -fdata-sections
ASFLAGS += -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT)
CFLAGS += -Wall
CFLAGS += -fdata-sections
CFLAGS += -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32F103CBTx_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) 
LDFLAGS += -Wl,-Map=$(BIN_DIR)/$(TARGET).map,--cref 
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,--print-memory-usage

include module.mk

test:
	@echo + modules $(MODULE_DIRS)
	@echo + C_src $(C_SOURCES)
	@echo + Incl $(C_INCLUDES)

# default action: build all
all: $(BIN_DIR)/$(TARGET).elf $(BIN_DIR)/$(TARGET).hex $(BIN_DIR)/$(TARGET).bin

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	@echo "Compiling $<"; $(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	@echo "Compiling $<"; $(AS) -c $(CFLAGS) $< -o $@

$(BIN_DIR)/$(TARGET).elf: $(OBJECTS) Makefile | $(BIN_DIR)
	@echo "+ Linking $@"; $(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BIN_DIR)/%.hex: $(BIN_DIR)/%.elf | $(BIN_DIR)
	@echo "+ Extracting $@"; $(HEX) $< $@
	
$(BIN_DIR)/%.bin: $(BIN_DIR)/%.elf | $(BIN_DIR)
	@echo "+ Extracting $@"; $(BIN) $< $@
	
$(BUILD_DIR):
	mkdir $@

$(BIN_DIR):
	mkdir $@

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)/*
	-rm -fr	$(BIN_DIR)/*

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
