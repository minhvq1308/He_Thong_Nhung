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

all: build/Ex1.bin

build/Ex1.o: src/Ex1.c inc/stm32f10x.h
	$(CC) $(CFLAGS) -c src/Ex1.c -o build/Ex1.o

build/startup.o: startup.s
	$(CC) -mcpu=cortex-m3 -mthumb -c startup.s -o build/startup.o

build/Ex1.elf: build/Ex1.o build/startup.o linker.ld
	$(CC) $(LDFLAGS) -o build/Ex1.elf \
		build/Ex1.o \
		build/startup.o

build/Ex1.bin: build/Ex1.elf
	$(OBJCOPY) -O binary build/Ex1.elf build/Ex1.bin

flash: build/Ex1.bin
	st-flash write build/Ex1.bin 0x08000000
	st-flash reset

clean:
	rm -f build/*.o
	rm -f build/*.elf
	rm -f build/*.bin