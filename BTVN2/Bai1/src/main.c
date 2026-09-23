#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include <stdio.h>
#include <string.h>

// ========================================================
// TÙY CHỈNH MÃ LỚP VÀ MÃ NHÓM CỦA BẠN TẠI ĐÂY
// ========================================================
#define MA_LOP   "D23CQDT01-B"
#define MA_NHOM  "N05"

#define RX_BUFFER_SIZE 256

// Bộ đệm nhận dữ liệu UART và các cờ điều khiển
volatile char rx_buffer[RX_BUFFER_SIZE];
volatile uint16_t rx_index = 0;
volatile uint8_t flag_complete = 0;

/**
  * @brief Cấu hình USART1 ở chế độ TX và RX (Baudrate 9600, 8N1)
  *        Chân sử dụng: PA9 (TX) - PA10 (RX)
  */
void USART1_Init(uint32_t baudrate) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 1. Cấp xung Clock cho USART1, GPIOA và AFIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    // 2. Cấu hình chân PA9 (TX) - Alternate Function Push-Pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 3. Cấu hình chân PA10 (RX) - Input Floating
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 4. Cấu hình thông số giao tiếp USART1
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // Bật cả RX và TX
    USART_Init(USART1, &USART_InitStructure);

    // 5. Cho phép ngắt khi nhận được 1 byte dữ liệu (RXNE)
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // 6. Cấu hình NVIC quản lý ngắt USART1
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 7. Bật mô-đun USART1
    USART_Cmd(USART1, ENABLE);
}

/**
  * @brief Gửi 1 ký tự qua USART1
  */
void USART1_SendChar(char ch) {
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, ch);
}

/**
  * @brief Gửi chuỗi ký tự qua USART1
  */
void USART1_SendString(const char *str) {
    while (*str) {
        USART1_SendChar(*str++);
    }
}

/**
  * @brief Trình xử lý ngắt USART1
  */
void USART1_IRQHandler(void) {
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        char ch = (char)USART_ReceiveData(USART1);

        if (!flag_complete) {
            if (ch == '!') {
                rx_buffer[rx_index] = '\0'; // Đóng chuỗi khi gặp ký tự '!'
                flag_complete = 1;         // Đánh dấu đã nhận trọn vẹn bản tin
            } else if (rx_index < RX_BUFFER_SIZE - 1) {
                rx_buffer[rx_index++] = ch; // Lưu ký tự nhận được vào bộ đệm
            }
        }
    }
}

int main(void) {
    // Cấu hình nhóm ưu tiên ngắt
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    // Khởi tạo USART1 với baudrate 9600 bps
    USART1_Init(9600);

    char tx_buffer[RX_BUFFER_SIZE + 64];

    while (1) {
        // Khi đã nhận đủ bản tin (kết thúc bằng ký tự '!')
        if (flag_complete) {
            // Tạo chuỗi phản hồi dạng: <Mã lớp><Mã nhóm>: <Bản tin đã nhận từ PC>\n\r
            snprintf(tx_buffer, sizeof(tx_buffer), "<%s><%s>: %s\n\r", MA_LOP, MA_NHOM, (char *)rx_buffer);

            // Gửi dữ liệu phản hồi về PC qua UART
            USART1_SendString(tx_buffer);

            // Reset bộ đệm và cờ trạng thái để chuẩn bị nhận bản tin tiếp theo
            rx_index = 0;
            flag_complete = 0;
        }
    }
}
