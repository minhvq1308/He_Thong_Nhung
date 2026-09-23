#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

// Biến đếm SysTick (đơn vị: ms)
volatile uint32_t ms_count_pb0 = 0; // 0.1Hz
volatile uint32_t ms_count_pb1 = 0; // 1Hz
volatile uint32_t ms_count_pb5 = 0; // 10Hz

/**
  * @brief Khởi tạo SysTick ngắt 1ms
  */
void SysTick_Init(void) {
    // SystemCoreClock = 72MHz -> 72000000 / 1000 = 72000 xung (đúng 1ms ngắt 1 lần)
    if (SysTick_Config(SystemCoreClock / 1000)) {
        while (1);
    }
}

/**
  * @brief Cấu hình GPIO cho 3 LED (PB0, PB1, PB5)
  */
void LED_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // Bật Clock GPIOB
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // Cấu hình PB0, PB1, PB5 là Output Push-Pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * @brief Trình xử lý ngắt SysTick (Tự động gọi mỗi 1ms)
  */
void SysTick_Handler(void) {
    // 1. LED PB0 - 0.1Hz (Chu kỳ T = 10s => Đảo trạng thái mỗi 5000ms)
    ms_count_pb0++;
    if (ms_count_pb0 >= 5000) {
        GPIOB->ODR ^= GPIO_Pin_0;
        ms_count_pb0 = 0;
    }

    // 2. LED PB1 - 1Hz (Chu kỳ T = 1s => Đảo trạng thái mỗi 500ms)
    ms_count_pb1++;
    if (ms_count_pb1 >= 500) {
        GPIOB->ODR ^= GPIO_Pin_1;
        ms_count_pb1 = 0;
    }

    // 3. LED PB5 - 10Hz (Chu kỳ T = 0.1s => Đảo trạng thái mỗi 50ms)
    ms_count_pb5++;
    if (ms_count_pb5 >= 50) {
        GPIOB->ODR ^= GPIO_Pin_5;
        ms_count_pb5 = 0;
    }
}

int main(void) {
    // Khởi tạo phần cứng
    LED_Init();
    SysTick_Init();

    // Vòng lặp chính trống vì SysTick tự động chạy ngắt ở nền
    while (1) {
    }
}
