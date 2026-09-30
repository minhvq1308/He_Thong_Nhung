#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include <stdio.h>

#define ADC_BUF_SIZE 100        // Lấy 100 mẫu/giây (Tần số 100Hz)
#define HALF_BUF_SIZE (ADC_BUF_SIZE / 2) // Nửa bộ đệm (50 mẫu)

// Bộ đệm chứa dữ liệu ADC tự động lưu bởi DMA
uint16_t adc_buffer[ADC_BUF_SIZE];

// Cờ báo dữ liệu sẵn sàng gửi qua UART
volatile uint8_t flag_send_first_half = 0;
volatile uint8_t flag_send_second_half = 0;

/* --- 1. KHỞI TẠO LED BÁO TRẠNG THÁI (PA3) --- */
void LED_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

// Hàm đảo trạng thái LED PA3
void LED_Toggle(void) {
    GPIOA->ODR ^= GPIO_Pin_3;
}

/* --- 2. KHỞI TẠO USART1 (PA9: TX, PA10: RX, Baudrate: 115200) --- */
void USART1_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // PA9 - USART1 TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // PA10 - USART1 RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    USART_Cmd(USART1, ENABLE);
}

void USART1_SendChar(char ch) {
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, ch);
}

void USART1_SendString(const char *str) {
    while (*str) {
        USART1_SendChar(*str++);
    }
}

/* --- 3. KHỞI TẠO TIMER 3 VỚI TẦN SỐ 100Hz (TẠO TRIGGER CHO ADC) --- */
void TIM3_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // Tần số Timer = 72MHz / (719 + 1) / (999 + 1) = 100 Hz
    TIM_TimeBaseStructure.TIM_Prescaler = 719;
    TIM_TimeBaseStructure.TIM_Period = 999;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // Xuất xung Trigger TRGO mỗi khi Timer tràn (Update Event)
    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);

    TIM_Cmd(TIM3, ENABLE);
}

/* --- 4. KHỞI TẠO ADC1 DÙNG CHÂN PA4 (CHANNEL 4) --- */
void ADC1_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); // ADCCLK = 72MHz / 6 = 12MHz

    // Cấu hình chân PA4 ở chế độ Analog Input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; // Kích hoạt bởi Timer TRGO
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO; // External Trigger từ TIM3
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    // Cấu hình Kênh 4 (PA4) với thời gian lấy mẫu 55.5 cycles
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_55Cycles5);

    // Cho phép External Trigger và DMA
    ADC_ExternalTrigConvCmd(ADC1, ENABLE);
    ADC_DMACmd(ADC1, ENABLE);

    ADC_Cmd(ADC1, ENABLE);

    // Hiệu chuẩn ADC
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
}

/* --- 5. KHỞI TẠO DMA1 CHANNEL 1 (CIRCULAR BUFFER VỚI NGẮT HT & TC) --- */
void DMA1_Init(void) {
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)adc_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = ADC_BUF_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 16-bit
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // Chế độ vòng lặp
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    // Cho phép ngắt Half-Transfer (HT) và Transfer-Complete (TC)
    DMA_ITConfig(DMA1_Channel1, DMA_IT_HT | DMA_IT_TC, ENABLE);

    // Cấu hình NVIC quản lý ngắt DMA1 Channel 1
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_Cmd(DMA1_Channel1, ENABLE);
}

/* --- 6. TRÌNH XỬ LÝ NGẮT DMA1 CHANNEL 1 --- */
void DMA1_Channel1_IRQHandler(void) {
    // Ngắt Half-Transfer: Đã ghi xong 50 mẫu đầu (index 0 -> 49)
    if (DMA_GetITStatus(DMA1_IT_HT1) != RESET) {
        DMA_ClearITPendingBit(DMA1_IT_HT1);
        flag_send_first_half = 1;
        LED_Toggle(); // Đảo trạng thái LED PA3 báo hiệu
    }

    // Ngắt Transfer-Complete: Đã ghi xong 50 mẫu sau (index 50 -> 99)
    if (DMA_GetITStatus(DMA1_IT_TC1) != RESET) {
        DMA_ClearITPendingBit(DMA1_IT_TC1);
        flag_send_second_half = 1;
        LED_Toggle(); // Đảo trạng thái LED PA3 báo hiệu
    }
}

/* --- HÀM TRUYỀN DỮ LIỆU SỐ LÊN PC QUA UART (NGẮT NHAU BỞI \n\r) --- */
void Send_Data_Block(uint16_t start_index, uint16_t length) {
    char str_buf[16];
    for (uint16_t i = 0; i < length; i++) {
        // Đúng định dạng số + \n\r theo yêu cầu đề bài
        snprintf(str_buf, sizeof(str_buf), "%d\n\r", adc_buffer[start_index + i]);
        USART1_SendString(str_buf);
    }
}

int main(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    LED_Init();
    USART1_Init();
    ADC1_Init();
    DMA1_Init();
    TIM3_Init(); // Bật Timer sau cùng để bắt đầu kích hoạt chuyển đổi

    while (1) {
        // Truyền 50 mẫu đầu tiên khi có ngắt Half-Transfer
        if (flag_send_first_half) {
            flag_send_first_half = 0;
            Send_Data_Block(0, HALF_BUF_SIZE);
        }

        // Truyền 50 mẫu tiếp theo khi có ngắt Transfer-Complete
        if (flag_send_second_half) {
            flag_send_second_half = 0;
            Send_Data_Block(HALF_BUF_SIZE, HALF_BUF_SIZE);
        }
    }
}