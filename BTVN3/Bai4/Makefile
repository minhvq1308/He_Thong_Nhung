CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

CFLAGS = -mcpu=cortex-m3 -mthumb -Wall -O0 -g \
         -Iinc \
         -ILibrary/inc \
         -DSTM32F10X_MD \
         -DUSE_STDPERIPH_DRIVER

LDFLAGS = -T stm32f103c8tx.ld -nostartfiles -Wl,--gc-sections \
          --specs=nano.specs \
          --specs=nosys.specs

SRCS = $(wildcard src/*.c) \
       $(wildcard Library/src/*.c) \
       startup_stm32f10x_md.s

all: build/main.elf build/main.hex build/main.bin
	@echo "================================="
	@echo "  BIÊN DỊCH THÀNH CÔNG!"
	@echo "================================="
	$(SIZE) build/main.elf

build/main.elf: $(SRCS)
	mkdir -p build
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -o $@

build/main.hex: build/main.elf
	$(OBJCOPY) -O ihex $< $@

build/main.bin: build/main.elf
	$(OBJCOPY) -O binary $< $@

# Quy tắc nạp code xuống kit qua OpenOCD
flash: build/main.elf
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program build/main.elf verify reset exit"

clean:
	rm -rf build

.PHONY: all clean flash
