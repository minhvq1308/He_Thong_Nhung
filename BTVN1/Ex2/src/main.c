#include <stdint.h>

#define RCC_APB2ENR (*(volatile uint32_t *)0x40021018)
#define GPIOA_CRL   (*(volatile uint32_t *)0x40010800)
#define GPIOA_ODR   (*(volatile uint32_t *)0x4001080C)

void delay(volatile uint32_t count)
{
    while (count--)
    {
        __asm__("nop");
    }
}

int main(void)
{
    int i;

    // Bật clock GPIOA
    RCC_APB2ENR |= (1 << 2);

    // PA0 -> PA7: Output Push-Pull, 2 MHz
    GPIOA_CRL = 0x22222222;

    while (1)
    {
        // Chạy từ trái sang phải
        for (i = 0; i < 8; i++)
        {
            GPIOA_ODR = (1 << i);
            delay(500000);
        }

        // Chạy từ phải sang trái
        for (i = 6; i >= 0; i--)
        {
            GPIOA_ODR = (1 << i);
            delay(500000);
        }
    }
}