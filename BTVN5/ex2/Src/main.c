#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdint.h>

/* BAI 02: PA0 EXTI -> Queue -> UART Task -> USART1 PA9 */

#define BTN_PIN             GPIO_PIN_0
#define QUEUE_LEN           8U
#define UART_TASK_STACK     192U
#define UART_TASK_PRIO      2U
#define DEBOUNCE_TICKS      pdMS_TO_TICKS(200U)

static UART_HandleTypeDef huart1;
static QueueHandle_t xCountQueue;

static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void vUartTask(void *pvParameters);
static uint16_t FormatCount(char *out, uint32_t n);
static void Error_Handler(void);

/* FreeRTOSConfig.h chua map xPortSysTickHandler sang SysTick_Handler */
extern void xPortSysTickHandler(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_USART1_UART_Init();

    xCountQueue = xQueueCreate(QUEUE_LEN, sizeof(uint32_t));
    if (xCountQueue == NULL)
    {
        Error_Handler();
    }

    if (xTaskCreate(vUartTask,
                    "UartTask",
                    UART_TASK_STACK,
                    NULL,
                    UART_TASK_PRIO,
                    NULL) != pdPASS)
    {
        Error_Handler();
    }

    /* EXTI priority must allow FreeRTOS FromISR APIs */
    HAL_NVIC_SetPriority(EXTI0_IRQn, 5U, 0U);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

    vTaskStartScheduler();

    /* Reached only if scheduler cannot start */
    Error_Handler();
    return 0;
}

static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    osc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    osc.HSEState = RCC_HSE_ON;
    osc.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    osc.HSIState = RCC_HSI_ON;
    osc.PLL.PLLState = RCC_PLL_ON;
    osc.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    osc.PLL.PLLMUL = RCC_PLL_MUL9;

    if (HAL_RCC_OscConfig(&osc) != HAL_OK)
    {
        Error_Handler();
    }

    clk.ClockType = RCC_CLOCKTYPE_HCLK |
                    RCC_CLOCKTYPE_SYSCLK |
                    RCC_CLOCKTYPE_PCLK1 |
                    RCC_CLOCKTYPE_PCLK2;
    clk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV2;
    clk.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* PA0 button to GND; internal pull-up; falling-edge interrupt */
    gpio.Pin = BTN_PIN;
    gpio.Mode = GPIO_MODE_IT_FALLING;
    gpio.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &gpio);
}

static void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200U;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
}

/* HAL calls this during HAL_UART_Init */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio = {0};

    if (huart->Instance == USART1)
    {
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* PA9 = USART1_TX, PA10 = USART1_RX */
        gpio.Pin = GPIO_PIN_9;
        gpio.Mode = GPIO_MODE_AF_PP;
        gpio.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &gpio);

        gpio.Pin = GPIO_PIN_10;
        gpio.Mode = GPIO_MODE_INPUT;
        gpio.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &gpio);
    }
}

/* Called by HAL_GPIO_EXTI_IRQHandler from stm32f1xx_it.c */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    static uint32_t count = 0U;
    static TickType_t lastTick = 0U;
    static uint8_t firstPress = 1U;

    BaseType_t higherPriorityTaskWoken = pdFALSE;
    TickType_t now;

    if (GPIO_Pin != BTN_PIN)
    {
        return;
    }

    now = xTaskGetTickCountFromISR();

    if ((firstPress == 0U) &&
        ((TickType_t)(now - lastTick) < DEBOUNCE_TICKS))
    {
        return;
    }

    firstPress = 0U;
    lastTick = now;
    count++;

    /* Send press count from ISR to UART task */
    (void)xQueueSendFromISR(xCountQueue,
                            &count,
                            &higherPriorityTaskWoken);

    portYIELD_FROM_ISR(higherPriorityTaskWoken);
}

static void vUartTask(void *pvParameters)
{
    uint32_t count;
    char buffer[32];
    uint16_t len;

    (void)pvParameters;

    {
        static const uint8_t ready[] = "Ready - press PA0\r\n";
        (void)HAL_UART_Transmit(&huart1,
                                (uint8_t *)ready,
                                sizeof(ready) - 1U,
                                HAL_MAX_DELAY);
    }

    for (;;)
    {
        if (xQueueReceive(xCountQueue, &count, portMAX_DELAY) == pdPASS)
        {
            len = FormatCount(buffer, count);
            (void)HAL_UART_Transmit(&huart1,
                                    (uint8_t *)buffer,
                                    len,
                                    HAL_MAX_DELAY);
        }
    }
}

static uint16_t FormatCount(char *out, uint32_t n)
{
    static const char prefix[] = "So lan nhan nut: ";
    char digits[10];
    uint16_t pos = 0U;
    uint16_t digitCount = 0U;

    while (pos < (sizeof(prefix) - 1U))
    {
        out[pos] = prefix[pos];
        pos++;
    }

    do
    {
        digits[digitCount] = (char)('0' + (n % 10U));
        digitCount++;
        n /= 10U;
    } while ((n != 0U) && (digitCount < sizeof(digits)));

    while (digitCount > 0U)
    {
        digitCount--;
        out[pos] = digits[digitCount];
        pos++;
    }

    out[pos++] = '\r';
    out[pos++] = '\n';

    return pos;
}

/* FreeRTOS owns the SysTick timing for this project.
   HAL tick is handled by hal_timebase_tim.c. */

static void Error_Handler(void)
{
    __disable_irq();

    for (;;)
    {
    }
}
