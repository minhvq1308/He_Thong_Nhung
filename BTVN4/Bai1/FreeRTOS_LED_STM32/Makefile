.RECIPEPREFIX = >

TARGET   = led_freertos
CC       = arm-none-eabi-gcc
OBJCOPY  = arm-none-eabi-objcopy
SIZE     = arm-none-eabi-size
LIB      = Libraries

CFLAGS   = -mcpu=cortex-m3 -mthumb -Os -g -Wall -ffunction-sections -fdata-sections \
           -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER
ASFLAGS  = -mcpu=cortex-m3 -mthumb
LDFLAGS  = -mcpu=cortex-m3 -mthumb -T linker.ld -Wl,--gc-sections -specs=nano.specs -specs=nosys.specs

# Tu dong tim moi thu muc co file .h trong Libraries
INC = -I. $(addprefix -I,$(sort $(dir $(shell find $(LIB) -name '*.h'))))

# Tu dong tim cac file .c can thiet
SRCS = main.c \
  $(shell find $(LIB) -name system_stm32f10x.c) \
  $(shell find $(LIB) \( -name stm32f10x_rcc.c -o -name stm32f10x_gpio.c -o -name misc.c \)) \
  $(shell find $(LIB) \( -name tasks.c -o -name list.c -o -name queue.c -o -name heap_4.c \)) \
  $(shell find $(LIB) -path '*GCC/ARM_CM3/port.c')

OBJS = $(SRCS:.c=.o) startup.o

all: $(TARGET).bin

%.o: %.c
> $(CC) $(CFLAGS) $(INC) -c $< -o $@

startup.o: startup.s
> $(CC) $(ASFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS)
> $(CC) $(OBJS) $(LDFLAGS) -o $@
> $(SIZE) $@

$(TARGET).bin: $(TARGET).elf
> $(OBJCOPY) -O binary $< $@

flash: $(TARGET).bin
> st-flash write $(TARGET).bin 0x08000000

clean:
> rm -f $(OBJS) $(TARGET).elf $(TARGET).bin

.PHONY: all flash clean
