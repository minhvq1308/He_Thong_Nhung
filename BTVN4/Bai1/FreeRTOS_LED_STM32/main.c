#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "FreeRTOS.h"
#include "task.h"

typedef struct {
    GPIO_TypeDef *port;
    uint16_t      pin;
    float         freq_hz;
} LedParam_t;

static const LedParam_t led1 = { GPIOA, GPIO_Pin_0, 0.1f  };
static const LedParam_t led2 = { GPIOA, GPIO_Pin_1, 1.0f  };
static const LedParam_t led3 = { GPIOA, GPIO_Pin_2, 10.0f };

/* 01 ham dieu khien nhay LED: dau vao gom port, pin, tan so (Hz) */
void LED_Blink(GPIO_TypeDef *port, uint16_t pin, float freq_hz)
{
    TickType_t half_period = (TickType_t)(500.0f / freq_hz) / portTICK_PERIOD_MS;
    if (half_period == 0) half_period = 1;

    if (GPIO_ReadOutputDataBit(port, pin))
        GPIO_ResetBits(port, pin);
    else
        GPIO_SetBits(port, pin);

    vTaskDelay(half_period);
}

static void vLedTask(void *pvParameters)
{
    const LedParam_t *p = (const LedParam_t *)pvParameters;
    for (;;)
    {
        LED_Blink(p->port, p->pin, p->freq_hz);
    }
}

static void GPIO_Config(void)
{
    GPIO_InitTypeDef g;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    g.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
    g.GPIO_Mode  = GPIO_Mode_Out_PP;
    g.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &g);
}

int main(void)
{
    SystemInit();
    SystemCoreClockUpdate();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    GPIO_Config();

    xTaskCreate(vLedTask, "LED_0.1Hz", 128, (void *)&led1, 1, NULL);
    xTaskCreate(vLedTask, "LED_1Hz",   128, (void *)&led2, 1, NULL);
    xTaskCreate(vLedTask, "LED_10Hz",  128, (void *)&led3, 1, NULL);

    vTaskStartScheduler();

    for (;;) { }
}

void vApplicationStackOverflowHook(TaskHandle_t t, char *name) { for (;;) {} }
void vApplicationMallocFailedHook(void)                         { for (;;) {} }
