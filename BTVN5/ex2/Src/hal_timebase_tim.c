#include "stm32f1xx_hal.h"

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    uint32_t timerClock;

    __HAL_RCC_TIM2_CLK_ENABLE();

    TIM2->CR1 = 0;
    TIM2->DIER = 0;
    TIM2->SR = 0;

    timerClock = HAL_RCC_GetPCLK1Freq();

    /* Timer clock = 2 * PCLK1 khi APB1 prescaler khac DIV1 */
    if ((RCC->CFGR & RCC_CFGR_PPRE1) != RCC_CFGR_PPRE1_DIV1)
    {
        timerClock *= 2U;
    }

    if (timerClock < 10000U)
    {
        return HAL_ERROR;
    }

    TIM2->PSC = (timerClock / 10000U) - 1U;
    TIM2->ARR = 9U;  /* 10 kHz / 10 = 1 kHz, moi tick 1 ms */
    TIM2->EGR = TIM_EGR_UG;
    TIM2->SR = 0;
    TIM2->DIER = TIM_DIER_UIE;

    HAL_NVIC_SetPriority(TIM2_IRQn, TickPriority, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->CR1 = TIM_CR1_CEN;

    return HAL_OK;
}

void TIM2_IRQHandler(void)
{
    if ((TIM2->SR & TIM_SR_UIF) != 0U)
    {
        TIM2->SR &= ~TIM_SR_UIF;
        HAL_IncTick();
    }
}
