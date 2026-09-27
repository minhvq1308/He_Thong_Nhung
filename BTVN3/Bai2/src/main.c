#include "stm32f10x.h"
#include "st7735.h"


/* ============================================================
 * Delay
 * ============================================================ */

static void DelayMs(uint32_t ms)
{
    uint32_t i;
    uint32_t j;

    for (i = 0; i < ms; i++)
    {
        for (j = 0; j < 8000; j++)
        {
            __NOP();
        }
    }
}


/* ============================================================
 * MAIN
 * ============================================================ */

int main(void)
{
    /* Khởi tạo hệ thống STM32 */

    SystemInit();


    /* Khởi tạo TFT ST7735 */

    ST7735_Init();


    while (1)
    {
        /* Màn hình đỏ */

        ST7735_FillScreen(ST7735_RED);

        DelayMs(1000);


        /* Màn hình xanh lá */

        ST7735_FillScreen(ST7735_GREEN);

        DelayMs(1000);


        /* Màn hình xanh dương */

        ST7735_FillScreen(ST7735_BLUE);

        DelayMs(1000);


        /* Màn hình trắng */

        ST7735_FillScreen(ST7735_WHITE);

        DelayMs(1000);


        /* Màn hình đen */

        ST7735_FillScreen(ST7735_BLACK);

        DelayMs(1000);
    }
}