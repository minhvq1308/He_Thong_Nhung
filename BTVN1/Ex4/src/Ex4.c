#include "stm32f10x.h"

int main(void)
{
    RCC_APB2ENR |= 0x00000004;
    RCC_APB2ENR |= 0x00000008;

    GPIOA_CRL &= 0xFFFFFFF0;
    GPIOA_CRL |= 0x00000008;
    GPIOA_ODR |= 0x00000001;

    GPIOB_CRL &= 0xFFFFFFF0;
    GPIOB_CRL |= 0x00000002;

    GPIOB_ODR &= 0xFFFFFFFE;

    int button_old = 1;
    int button_pressed = 0;

    while (1)
    {
        int button_now = GPIOA_IDR & 0x00000001;

        if (button_now == 0 && button_old == 1)
        {
            button_pressed = 1;
        }

        if (button_now == 1 && button_old == 0 && button_pressed == 1)
        {
            GPIOB_ODR ^= 0x00000001;
            button_pressed = 0;
        }

        button_old = button_now;
    }
}