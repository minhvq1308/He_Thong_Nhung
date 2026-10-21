
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

/* Ngat ngoai PA0 */
extern void xPortSysTickHandler(void);
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

/* Tick cua HAL va FreeRTOS */
void SysTick_Handler(void)
{
    HAL_IncTick();

    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        xPortSysTickHandler();
    }
}

