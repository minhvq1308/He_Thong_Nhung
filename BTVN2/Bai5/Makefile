TARGET = Bai5
BUILD  = build

CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

CPU = -mcpu=cortex-m3 -mthumb

CFLAGS = $(CPU)
CFLAGS += -std=c11
CFLAGS += -O0
CFLAGS += -g3
CFLAGS += -Wall
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections

CFLAGS += -DSTM32F10X_MD
CFLAGS += -DUSE_STDPERIPH_DRIVER

CFLAGS += -Iinc
CFLAGS += -ILibraries/CMSIS/CM3/CoreSupport
CFLAGS += -ILibraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
CFLAGS += -ILibraries/STM32F10x_StdPeriph_Driver/inc

LDFLAGS = $(CPU)
LDFLAGS += -TSTM32F103C8Tx_FLASH.ld
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,-Map=$(BUILD)/$(TARGET).map
LDFLAGS += --specs=nano.specs
LDFLAGS += --specs=nosys.specs

C_SOURCES = \
    src/main.c \
    src/stm32f10x_it.c \
    src/system_stm32f10x.c \
    Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c \
    Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c \
    Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_tim.c \
    Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c \
    Libraries/STM32F10x_StdPeriph_Driver/src/misc.c

ASM_SOURCES = \
    startup_stm32f10x_md.s

C_OBJECTS = $(C_SOURCES:%.c=$(BUILD)/%.o)
ASM_OBJECTS = $(ASM_SOURCES:%.s=$(BUILD)/%.o)

OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

all: $(BUILD)/$(TARGET).bin

$(BUILD)/$(TARGET).elf: $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) $^ -o $@
	$(SIZE) $@

$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(CPU) -c $< -o $@

clean:
	rm -rf $(BUILD)

-include $(C_OBJECTS:.o=.d)

.PHONY: all clean
