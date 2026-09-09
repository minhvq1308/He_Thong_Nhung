TARGET = main

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

CPU = -mcpu=cortex-m3
MCU = $(CPU) -mthumb

CFLAGS = $(MCU) -Wall -Wextra -O0 \
         -ffunction-sections -fdata-sections \
         -ffreestanding

LDFLAGS = $(MCU) -Tlinker.ld -nostdlib \
          -Wl,--gc-sections

OBJS = build/main.o build/startup.o

all: build/$(TARGET).elf build/$(TARGET).bin build/$(TARGET).hex

build:
	mkdir -p build

build/main.o: src/main.c | build
	$(CC) $(CFLAGS) -c src/main.c -o build/main.o

build/startup.o: startup.s | build
	$(CC) $(MCU) -c startup.s -o build/startup.o

build/$(TARGET).elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o build/$(TARGET).elf
	$(SIZE) build/$(TARGET).elf

build/$(TARGET).bin: build/$(TARGET).elf
	$(OBJCOPY) -O binary build/$(TARGET).elf build/$(TARGET).bin

build/$(TARGET).hex: build/$(TARGET).elf
	$(OBJCOPY) -O ihex build/$(TARGET).elf build/$(TARGET).hex

flash: build/$(TARGET).bin
	st-flash write build/$(TARGET).bin 0x08000000
	st-flash reset

clean:
	rm -rf build
