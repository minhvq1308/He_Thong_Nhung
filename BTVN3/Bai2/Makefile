# ============================================================
# BTVN3 - Bai2
# STM32F103C8T6 + SPI1 + ST7735
# Standard Peripheral Library
# ============================================================

TARGET = Bai2

CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

CPU = cortex-m3

BUILD_DIR = build
SRC_DIR   = src
INC_DIR   = inc

SPL_DIR   = Libraries/STM32F10x_StdPeriph_Driver
CMSIS_DIR = Libraries/CMSIS


# ============================================================
# Compiler flags
# ============================================================

CFLAGS = \
	-mcpu=$(CPU) \
	-mthumb \
	-O0 \
	-ffreestanding \
	-Wall \
	-DSTM32F10X_MD \
	-DUSE_STDPERIPH_DRIVER


# ============================================================
# Include directories
# ============================================================

CFLAGS += \
	-I$(INC_DIR) \
	-I$(SPL_DIR)/inc \
	-I$(CMSIS_DIR)/CM3/CoreSupport \
	-I$(CMSIS_DIR)/CM3/DeviceSupport/ST/STM32F10x


# ============================================================
# Linker flags
# ============================================================

LDFLAGS = \
	-mcpu=$(CPU) \
	-mthumb \
	-TSTM32F103C8Tx_FLASH.ld \
	-nostdlib \
	-Wl,-Map=$(BUILD_DIR)/$(TARGET).map


# ============================================================
# Source files của chương trình
# ============================================================

PROJECT_SRC = \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/st7735.c


# ============================================================
# Standard Peripheral Library
#
# Chỉ compile các module thực sự sử dụng:
# GPIO
# RCC
# SPI
# ============================================================

SPL_SRC = \
	$(SPL_DIR)/src/stm32f10x_gpio.c \
	$(SPL_DIR)/src/stm32f10x_rcc.c \
	$(SPL_DIR)/src/stm32f10x_spi.c


# ============================================================
# CMSIS
# ============================================================

CMSIS_SRC = \
	$(CMSIS_DIR)/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c


# ============================================================
# Object files
# ============================================================

PROJECT_OBJ = \
	$(BUILD_DIR)/main.o \
	$(BUILD_DIR)/st7735.o

SPL_OBJ = \
	$(BUILD_DIR)/stm32f10x_gpio.o \
	$(BUILD_DIR)/stm32f10x_rcc.o \
	$(BUILD_DIR)/stm32f10x_spi.o

CMSIS_OBJ = \
	$(BUILD_DIR)/system_stm32f10x.o

STARTUP_OBJ = \
	$(BUILD_DIR)/startup_stm32f10x_md.o


OBJECTS = \
	$(PROJECT_OBJ) \
	$(SPL_OBJ) \
	$(CMSIS_OBJ) \
	$(STARTUP_OBJ)


# ============================================================
# Default target
# ============================================================

all: $(BUILD_DIR)/$(TARGET).bin


# ============================================================
# Link ELF
# ============================================================

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS) $^ -o $@
	$(SIZE) $@


# ============================================================
# Convert ELF -> BIN
# ============================================================

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@


# ============================================================
# Compile main.c
# ============================================================

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


# ============================================================
# Compile st7735.c
# ============================================================

$(BUILD_DIR)/st7735.o: $(SRC_DIR)/st7735.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


# ============================================================
# Compile GPIO
# ============================================================

$(BUILD_DIR)/stm32f10x_gpio.o: $(SPL_DIR)/src/stm32f10x_gpio.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


# ============================================================
# Compile RCC
# ============================================================

$(BUILD_DIR)/stm32f10x_rcc.o: $(SPL_DIR)/src/stm32f10x_rcc.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


# ============================================================
# Compile SPI
# ============================================================

$(BUILD_DIR)/stm32f10x_spi.o: $(SPL_DIR)/src/stm32f10x_spi.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


# ============================================================
# Compile CMSIS system file
# ============================================================

$(BUILD_DIR)/system_stm32f10x.o: $(CMSIS_SRC)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


# ============================================================
# Compile startup
# ============================================================

$(BUILD_DIR)/startup_stm32f10x_md.o: startup_stm32f10x_md.s
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


# ============================================================
# Flash STM32
# ============================================================

flash: $(BUILD_DIR)/$(TARGET).bin
	st-flash write $(BUILD_DIR)/$(TARGET).bin 0x08000000
	st-flash reset


# ============================================================
# Clean
# ============================================================

clean:
	rm -rf $(BUILD_DIR)/*


# ============================================================
# Phony targets
# ============================================================

.PHONY: all flash clean