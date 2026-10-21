PREFIX  = arm-none-eabi-
CC      = $(PREFIX)gcc
SIZE    = $(PREFIX)size
OBJCOPY = $(PREFIX)objcopy

TARGET  = build/ex2

CPU     = -mcpu=cortex-m3 -mthumb
CFLAGS  = $(CPU) -O1 -g3 -std=gnu11 -Wall -Wextra \
          -ffunction-sections -fdata-sections -fno-common \
          -DUSE_HAL_DRIVER -DSTM32F103xB

INCLUDES = \
    -I. \
    -IDrivers/STM32F1xx_HAL_Driver/Inc \
    -IDrivers/STM32F1xx_HAL_Driver/Inc/Legacy \
    -IDrivers/CMSIS/Include \
    -IDrivers/CMSIS/Device/ST/STM32F1xx/Include \
    -IMiddlewares/Third_Party/FreeRTOS/Source/include \
    -IMiddlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3

LDFLAGS = $(CPU) -Tlinker.ld \
          -Wl,--gc-sections \
          -Wl,-Map=$(TARGET).map \
          --specs=nano.specs --specs=nosys.specs

HAL_SRC = \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c \
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c

FREERTOS_SRC = \
    Middlewares/Third_Party/FreeRTOS/Source/tasks.c \
    Middlewares/Third_Party/FreeRTOS/Source/queue.c \
    Middlewares/Third_Party/FreeRTOS/Source/list.c \
    Middlewares/Third_Party/FreeRTOS/Source/timers.c \
    Middlewares/Third_Party/FreeRTOS/Source/event_groups.c \
    Middlewares/Third_Party/FreeRTOS/Source/croutine.c \
    Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c \
    Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c \
    Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c

SRC = \
    Src/main.c \
    Src/stm32f1xx_it.c \
    Src/hal_timebase_tim.c \
    Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/system_stm32f1xx.c \
    $(HAL_SRC) \
    $(FREERTOS_SRC)

OBJ = $(SRC:.c=.o) startup_stm32f103xb.o

all: $(TARGET).elf $(TARGET).bin

$(TARGET).elf: $(OBJ) linker.ld
	$(CC) $(OBJ) $(LDFLAGS) -Wl,--start-group -lc -lm -lgcc -lnosys -Wl,--end-group -o $@
	$(SIZE) $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

startup_stm32f103xb.o: startup_stm32f103xb.s
	$(CC) $(CPU) -x assembler-with-cpp -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET).elf $(TARGET).bin $(TARGET).map

.PHONY: all clean

