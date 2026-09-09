
#include <stdint.h>

// RCC
#define RCC_APB2ENR (*(volatile uint32_t *)0x40021018)

// GPIOA
#define GPIOA_CRL   (*(volatile uint32_t *)0x40010800)
#define GPIOA_CRH   (*(volatile uint32_t *)0x40010804)
#define GPIOA_IDR   (*(volatile uint32_t *)0x40010808)
#define GPIOA_ODR   (*(volatile uint32_t *)0x4001080C)

int main(void)
{
    uint8_t data;

    // Bật clock GPIOA
    RCC_APB2ENR |= (1 << 2);

    // PA0 - PA7: Input floating
    GPIOA_CRL = 0x44444444;

    // PA8 - PA15: Output Push-Pull, 2 MHz
    GPIOA_CRH = 0x22222222;

    while (1)
    {
        // Đọc PA0 - PA7
        data = GPIOA_IDR & 0xFF;

        // Đảo 8 bit dữ liệu
        data = ~data;

        // Xuất sang PA8 - PA15
        GPIOA_ODR = ((uint32_t)data << 8);
    }
}
