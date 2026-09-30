TARGET = bai3_uart_dma

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CPU = -mcpu=cortex-m3 -mthumb

CFLAGS = $(CPU) \
         -O0 \
         -g \
         -Wall \
         -ffunction-sections \
         -fdata-sections \
         -DUSE_STDPERIPH_DRIVER \
         -DSTM32F10X_MD \
         -I./Libraries/CMSIS/CM3/CoreSupport \
         -I./Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x \
         -I./Libraries/STM32F10x_StdPeriph_Driver/inc

LDFLAGS = $(CPU) \
          -Tlinker.ld \
          -nostartfiles \
          -Wl,--gc-sections

BUILD = build

SRC = main.c \
      Libraries/STM32F10x_StdPeriph_Driver/src/misc.c \
      Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c \
      Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c \
      Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c \
      Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_dma.c \
      Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c

OBJ = $(SRC:%.c=$(BUILD)/%.o)

STARTUP = $(BUILD)/startup_stm32f10x_md.o

all: $(BUILD)/$(TARGET).bin

$(BUILD)/$(TARGET).elf: $(OBJ) $(STARTUP) linker.ld
	$(CC) $(LDFLAGS) $(STARTUP) $(OBJ) -lc -lm -lnosys -o $@

$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD)/$(TARGET).hex: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

$(BUILD)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/startup_stm32f10x_md.o: startup_stm32f10x_md.s
	mkdir -p $(dir $@)
	$(CC) $(CPU) -c $< -o $@

clean:
	rm -rf $(BUILD)/*

flash: $(BUILD)/$(TARGET).bin
	st-flash write $(BUILD)/$(TARGET).bin 0x08000000

hex: $(BUILD)/$(TARGET).hex

.PHONY: all clean flash hex
