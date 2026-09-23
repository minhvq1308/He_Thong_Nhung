TARGET = BAI3

CC = arm-none-eabi-gcc
AS = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# ==========================================
# Đường dẫn thư viện
# ==========================================

LIB = ../bai4/Libraries/STM32F10x_StdPeriph_Driver

CMSIS = ../bai4/STM32F10x-standard-peripheral-library-main/Libraries/CMSIS

SYSTEM = ../bai4/system_stm32f10x.c

STARTUP = ../bai4/startup_stm32f10x_md.s

CONF = ../bai4


# ==========================================
# Compiler flags
# ==========================================

CFLAGS = -mcpu=cortex-m3 \
         -mthumb \
         -mfloat-abi=soft \
         -DSTM32F10X_MD \
         -DUSE_STDPERIPH_DRIVER \
         -I. \
         -I$(CONF) \
         -I$(CMSIS)/CM3/CoreSupport \
         -I$(CMSIS)/CM3/DeviceSupport/ST/STM32F10x \
         -I$(LIB)/inc

CFLAGS += -O0 -g -Wall


# ==========================================
# Linker flags
# ==========================================

LDFLAGS = -Tlinker.ld \
          -mcpu=cortex-m3 \
          -mthumb \
          -mfloat-abi=soft \
          -nostdlib


# ==========================================
# Source files
# ==========================================

SRC = main.c \
      $(SYSTEM) \
      $(LIB)/src/stm32f10x_gpio.c \
      $(LIB)/src/stm32f10x_rcc.c \
      $(LIB)/src/stm32f10x_adc.c \
      $(LIB)/src/stm32f10x_usart.c

OBJ = $(SRC:.c=.o)

STARTUP_OBJ = startup_stm32f10x_md.o


# ==========================================
# Build
# ==========================================

all: $(TARGET).elf $(TARGET).bin


$(TARGET).elf: $(OBJ) $(STARTUP_OBJ)
	$(CC) $(OBJ) $(STARTUP_OBJ) $(LDFLAGS) -o $@


$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@


# ==========================================
# Compile C
# ==========================================

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


# ==========================================
# Compile startup
# ==========================================

$(STARTUP_OBJ): $(STARTUP)
	$(AS) $(CFLAGS) -c $< -o $@


# ==========================================
# Clean
# ==========================================

clean:
	rm -f main.o
	rm -f $(SYSTEM:.c=.o)
	rm -f $(LIB)/src/*.o
	rm -f $(STARTUP_OBJ)
	rm -f $(TARGET).elf
	rm -f $(TARGET).bin


# ==========================================
# Flash
# ==========================================

flash: $(TARGET).bin
	st-flash write $(TARGET).bin 0x08000000