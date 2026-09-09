TARGET = main

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

CPU = -mcpu=cortex-m3
MCU = $(CPU) -mthumb

CFLAGS = $(MCU) -Wall -O0 -ffunction-sections -fdata-sections

LDFLAGS = $(MCU) -Tlinker.ld -nostdlib -Wl,--gc-sections

OBJS = main.o startup.o

all: $(TARGET).elf $(TARGET).bin $(TARGET).hex

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

startup.o: startup.s
	$(CC) $(CFLAGS) -c startup.s -o startup.o

$(TARGET).elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET).elf
	$(SIZE) $(TARGET).elf

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $(TARGET).elf $(TARGET).hex

clean:
	rm -f *.o *.elf *.bin *.hex
