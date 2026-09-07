#include "stm32f10x.h"

#define DELAY 500000

void delay(void)
{
    for ( uint32_t i = 0; i < DELAY; i++)
    {
    }
}

int main(void)
{
    RCC_APB2ENR |= (1U << 4);


    GPIOC_CRH &= ~(0xFU << 20);
    GPIOC_CRH |= (0x2U << 20);

    while (1)
    {
        GPIOC_ODR ^= (1U << 13);

        delay();
    }
}