#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Định nghĩa cấu trúc dữ liệu truyền qua Queue */
typedef struct {
    uint8_t led_id;      // 1: LED 1 (PA0), 2: LED 2 (PA1), 3: LED 3 (PA2)
    uint16_t duty_cycle; // Giá trị PWM (0 - 1000 tương ứng 0% - 100%)
} LED_Msg_t;

/* Khai báo Queue Handle */
QueueHandle_t xLedQueue;

/* Biến lưu trạng thái hiện tại */
static uint8_t current_selected_led = 1;
static uint16_t led_duty[3] = {500, 500, 500}; // Mặc định 50% duty cycle

/* Khởi tạo ngoại vi theo chân mới */
void System_Peripherals_Init(void) {
    // Bật clock cho GPIOA, GPIOB và AFIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    // Bật clock cho Timer 2 (TIM2 cung cấp PWM cho PA0, PA1, PA2)
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* --- CẤU HÌNH PWM CHO 3 LED (TIM2: PA0 - CH1, PA1 - CH2, PA2 - CH3) --- */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Cấu hình Timer 2: Tần số 1kHz (Clock 72MHz -> Prescaler 71, ARR 999)
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 999;          // ARR
    TIM_TimeBaseStructure.TIM_Prescaler = 71;        // PSC (72MHz / 72 = 1MHz)
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // Cấu hình PWM Mode cho 3 Channel của TIM2
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 500; // Mức xung ban đầu
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC1Init(TIM2, &TIM_OCInitStructure); // LED 1 (PA0)
    TIM_OC2Init(TIM2, &TIM_OCInitStructure); // LED 2 (PA1)
    TIM_OC3Init(TIM2, &TIM_OCInitStructure); // LED 3 (PA2)

    TIM_Cmd(TIM2, ENABLE);

    /* --- CẤU HÌNH 3 NÚT NHẤN (PB12, PB14, PA8) --- */
    // Cấu hình PB12, PB14 trên cổng GPIOB
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // Input Pull-up (Nhấn xuống mức 0)
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Cấu hình PA8 trên cổng GPIOA
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/* ================= TASK 1: ĐỌC NÚT NHẤN ================= */
void vTaskButton(void *pvParameters) {
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(50); // Quét mỗi 50ms (chống rung cơ bản)
    xLastWakeTime = xTaskGetTickCount();

    uint8_t btn1_press = 0, btn2_press = 0, btn3_press = 0;

    while(1) {
        // Nút 1 (PB12): Chuyển LED được chọn
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == Bit_RESET) {
            if (!btn1_press) {
                btn1_press = 1;
                current_selected_led++;
                if (current_selected_led > 3) current_selected_led = 1;
            }
        } else {
            btn1_press = 0;
        }

        // Nút 2 (PB14): Tăng độ sáng LED hiện tại
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == Bit_RESET) {
            if (!btn2_press) {
                btn2_press = 1;
                if (led_duty[current_selected_led - 1] <= 900) {
                    led_duty[current_selected_led - 1] += 100; // Tăng 10%
                } else {
                    led_duty[current_selected_led - 1] = 1000;
                }
                
                // Gửi dữ liệu mới qua Queue
                LED_Msg_t msg = {
                    .led_id = current_selected_led,
                    .duty_cycle = led_duty[current_selected_led - 1]
                };
                xQueueSend(xLedQueue, &msg, 0);
            }
        } else {
            btn2_press = 0;
        }

        // Nút 3 (PA8): Giảm độ sáng LED hiện tại
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == Bit_RESET) {
            if (!btn3_press) {
                btn3_press = 1;
                if (led_duty[current_selected_led - 1] >= 100) {
                    led_duty[current_selected_led - 1] -= 100; // Giảm 10%
                } else {
                    led_duty[current_selected_led - 1] = 0;
                }
                
                // Gửi dữ liệu mới qua Queue
                LED_Msg_t msg = {
                    .led_id = current_selected_led,
                    .duty_cycle = led_duty[current_selected_led - 1]
                };
                xQueueSend(xLedQueue, &msg, 0);
            }
        } else {
            btn3_press = 0;
        }

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

/* ================= TASK 2, 3, 4: 3 TASK ĐIỀU KHIỂN 3 LED ĐỘC LẬP ================= */
void vTaskLED1(void *pvParameters) {
    LED_Msg_t rxMsg;
    while(1) {
        if (xQueueReceive(xLedQueue, &rxMsg, portMAX_DELAY) == pdPASS) {
            if (rxMsg.led_id == 1) {
                TIM_SetCompare1(TIM2, rxMsg.duty_cycle); // Cập nhật PWM Channel 1 (PA0)
            }
        }
    }
}

void vTaskLED2(void *pvParameters) {
    LED_Msg_t rxMsg;
    while(1) {
        if (xQueueReceive(xLedQueue, &rxMsg, portMAX_DELAY) == pdPASS) {
            if (rxMsg.led_id == 2) {
                TIM_SetCompare2(TIM2, rxMsg.duty_cycle); // Cập nhật PWM Channel 2 (PA1)
            }
        }
    }
}

void vTaskLED3(void *pvParameters) {
    LED_Msg_t rxMsg;
    while(1) {
        if (xQueueReceive(xLedQueue, &rxMsg, portMAX_DELAY) == pdPASS) {
            if (rxMsg.led_id == 3) {
                TIM_SetCompare3(TIM2, rxMsg.duty_cycle); // Cập nhật PWM Channel 3 (PA2)
            }
        }
    }
}

/* ================= HÀM MAIN ================= */
int main(void) {
    SystemInit();
    System_Peripherals_Init();

    // Tạo Queue chứa tối đa 10 thông điệp
    xLedQueue = xQueueCreate(10, sizeof(LED_Msg_t));

    if (xLedQueue != NULL) {
        // Tạo các Tasks FreeRTOS
        xTaskCreate(vTaskButton, "BtnTask", configMINIMAL_STACK_SIZE + 128, NULL, 2, NULL);
        xTaskCreate(vTaskLED1,   "LED1Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        xTaskCreate(vTaskLED2,   "LED2Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        xTaskCreate(vTaskLED3,   "LED3Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

        // Khởi động FreeRTOS Scheduler
        vTaskStartScheduler();
    }

    while(1);
}