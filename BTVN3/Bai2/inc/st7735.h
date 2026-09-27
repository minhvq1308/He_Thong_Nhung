#ifndef ST7735_H
#define ST7735_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"

/* ==============================
 * Màu RGB565
 * ============================== */

#define ST7735_BLACK    0x0000
#define ST7735_BLUE     0x001F
#define ST7735_RED      0xF800
#define ST7735_GREEN    0x07E0
#define ST7735_WHITE    0xFFFF
#define ST7735_YELLOW   0xFFE0
#define ST7735_CYAN     0x07FF
#define ST7735_MAGENTA  0xF81F

void ST7735_Init(void);
void ST7735_FillScreen(uint16_t color);

#endif