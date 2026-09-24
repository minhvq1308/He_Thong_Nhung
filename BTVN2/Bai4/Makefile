TARGET = BAI4

CC      = arm-none-eabi-gcc
AS      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# ==========================================
# LIBRARY
# ==========================================

LIB   = ./Libraries/STM32F10x_StdPeriph_Driver
CMSIS = ./Libraries/CMSIS

SYSTEM  = ./system_stm32f10x.c
STARTUP = ./startup_stm32f10x_md.s

# ==========================================
# COMPILER FLAGS
# ==========================================

CFLAGS = -mcpu=cortex-m3 \
         -mthumb \
         -mfloat-abi=soft \
         -DSTM32F10X_MD \
         -DUSE_STDPERIPH_DRIVER \
         -I. \
         -I$(CMSIS)/CM3/CoreSupport \
         -I$(CMSIS)/CM3/DeviceSupport/ST/STM32F10x \
         -I$(LIB)/inc

CFLAGS += -O0 -g -Wall

# ==========================================
# LINKER FLAGS
# ==========================================

LDFLAGS = -T./linker.ld \
          -mcpu=cortex-m3 \
          -mthumb \
          -mfloat-abi=soft \
          -nostdlib

# ==========================================
# SOURCE FILES
# ==========================================

SRC = main.c \
      $(SYSTEM) \
      $(LIB)/src/stm32f10x_gpio.c \
      $(LIB)/src/stm32f10x_rcc.c \
      $(LIB)/src/stm32f10x_tim.c \
      $(LIB)/src/stm32f10x_adc.c \
      $(LIB)/src/stm32f10x_usart.c

OBJ = $(SRC:.c=.o)

STARTUP_OBJ = startup_stm32f10x_md.o

# ==========================================
# BUILD
# ==========================================

all: $(TARGET).elf $(TARGET).bin

$(TARGET).elf: $(OBJ) $(STARTUP_OBJ)
	$(CC) $(OBJ) $(STARTUP_OBJ) $(LDFLAGS) -o $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# ==========================================
# COMPILE C
# ==========================================

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ==========================================
# COMPILE STARTUP
# ==========================================

$(STARTUP_OBJ): $(STARTUP)
	$(AS) $(CFLAGS) -c $< -o $@

# ==========================================
# CLEAN
# ==========================================

clean:
	rm -f main.o
	rm -f system_stm32f10x.o
	rm -f $(LIB)/src/*.o
	rm -f $(STARTUP_OBJ)
	rm -f $(TARGET).elf
	rm -f $(TARGET).bin

# ==========================================
# FLASH
# ==========================================

flash: $(TARGET).bin
	st-flash write $(TARGET).bin 0x08000000