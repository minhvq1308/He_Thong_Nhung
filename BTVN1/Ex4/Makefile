CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -mcpu=cortex-m3 \
         -mthumb \
         -O0 \
         -Wall \
         -Wextra \
         -ffreestanding \
         -Iinc

LDFLAGS = -mcpu=cortex-m3 \
          -mthumb \
          -nostdlib \
          -T linker.ld

all: build/Ex4.bin

build/Ex4.o: src/Ex4.c inc/stm32f10x.h
	$(CC) $(CFLAGS) -c src/Ex4.c -o build/Ex4.o

build/startup.o: startup.s
	$(CC) -mcpu=cortex-m3 -mthumb -c startup.s -o build/startup.o

build/Ex4.elf: build/Ex4.o build/startup.o linker.ld
	$(CC) $(LDFLAGS) -o build/Ex4.elf \
		build/Ex4.o \
		build/startup.o

build/Ex4.bin: build/Ex4.elf
	$(OBJCOPY) -O binary build/Ex4.elf build/Ex4.bin

flash: build/Ex4.bin
	st-flash write build/Ex4.bin 0x08000000
	st-flash reset

clean:
	rm -f build/*.o
	rm -f build/*.elf
	rm -f build/*.bin