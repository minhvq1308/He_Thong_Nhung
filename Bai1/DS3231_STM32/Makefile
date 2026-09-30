TARGET = ds3231

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

LIB = ./Libraries

CFLAGS = -mcpu=cortex-m3 \
         -mthumb \
         -DSTM32F10X_MD \
         -DUSE_STDPERIPH_DRIVER \
         -ffreestanding \
         -Wall \
         -Wextra \
         -O0 \
         -I. \
         -I$(LIB)/CMSIS/CM3/CoreSupport \
         -I$(LIB)/CMSIS/CM3/DeviceSupport/ST/STM32F10x \
         -I$(LIB)/STM32F10x_StdPeriph_Driver/inc
LDFLAGS = -Tlinker.ld \
          -nostdlib \
          -Wl,--gc-sections

OBJ = build/main.o \
      build/stm32f10x_gpio.o \
      build/stm32f10x_i2c.o \
      build/stm32f10x_rcc.o \
      build/stm32f10x_usart.o \
      build/system_stm32f10x.o \
      build/startup.o

all: build/$(TARGET).elf build/$(TARGET).bin

build:
	mkdir -p build

build/main.o: main.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build/stm32f10x_gpio.o: $(LIB)/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build/stm32f10x_i2c.o: $(LIB)/STM32F10x_StdPeriph_Driver/src/stm32f10x_i2c.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build/stm32f10x_rcc.o: $(LIB)/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build/stm32f10x_usart.o: $(LIB)/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build/system_stm32f10x.o: $(LIB)/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build/startup.o: startup.s | build
	$(CC) $(CFLAGS) -c $< -o $@

build/$(TARGET).elf: $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@
	$(SIZE) $@

build/$(TARGET).bin: build/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf build

flash: build/$(TARGET).bin
	st-flash write build/$(TARGET).bin 0x08000000
