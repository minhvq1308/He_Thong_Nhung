#include "main.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

volatile char rx_buffer[64];
volatile uint8_t rx_index = 0;

volatile char command[64];
volatile uint8_t command_ready = 0;

volatile uint8_t led_on = 0;
volatile uint8_t pwm_percent = 50;

void GPIO_Config(void);
void TIM2_PWM_Config(void);
void USART1_Config(void);

void USART1_SendChar(char c);
void USART1_SendString(const char *str);

void Set_PWM(uint8_t percent);
void Process_Command(char *cmd);

int main(void)
{
    SystemInit();

    GPIO_Config();
    TIM2_PWM_Config();
    USART1_Config();

    pwm_percent = 50;
    led_on = 0;

    Set_PWM(0);

    USART1_SendString("\r\n");
    USART1_SendString("STM32 BAI 5 READY\r\n");
    USART1_SendString("Commands:\r\n");
    USART1_SendString("ON!\r\n");
    USART1_SendString("OFF!\r\n");
    USART1_SendString("PWM:xx%!\r\n");
    USART1_SendString("Status!\r\n");
    USART1_SendString("=======================\r\n");

    while (1)
    {
        if (command_ready)
        {
            char local_command[64];

            __disable_irq();

            strcpy(
                local_command,
                (const char *)command
            );

            command_ready = 0;

            __enable_irq();

            Process_Command(local_command);
        }
    }
}

void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_AFIO,
        ENABLE
    );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(
        GPIOA,
        &GPIO_InitStructure
    );
}

void TIM2_PWM_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(
        RCC_APB1Periph_TIM2,
        ENABLE
    );

    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseStructure.TIM_CounterMode =
        TIM_CounterMode_Up;

    TIM_TimeBaseStructure.TIM_Period = 999;

    TIM_TimeBaseStructure.TIM_ClockDivision =
        TIM_CKD_DIV1;

    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(
        TIM2,
        &TIM_TimeBaseStructure
    );

    TIM_OCInitStructure.TIM_OCMode =
        TIM_OCMode_PWM1;

    TIM_OCInitStructure.TIM_OutputState =
        TIM_OutputState_Enable;

    TIM_OCInitStructure.TIM_Pulse = 0;

    TIM_OCInitStructure.TIM_OCPolarity =
        TIM_OCPolarity_High;

    TIM_OC1Init(
        TIM2,
        &TIM_OCInitStructure
    );

    TIM_OC1PreloadConfig(
        TIM2,
        TIM_OCPreload_Enable
    );

    TIM_ARRPreloadConfig(
        TIM2,
        ENABLE
    );

    TIM_Cmd(
        TIM2,
        ENABLE
    );
}

void USART1_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_AFIO |
        RCC_APB2Periph_USART1,
        ENABLE
    );

    /* PA9 = TX */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed =
        GPIO_Speed_50MHz;

    GPIO_Init(
        GPIOA,
        &GPIO_InitStructure
    );

    /* PA10 = RX */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode =
        GPIO_Mode_IN_FLOATING;

    GPIO_Init(
        GPIOA,
        &GPIO_InitStructure
    );

    USART_InitStructure.USART_BaudRate = 115200;

    USART_InitStructure.USART_WordLength =
        USART_WordLength_8b;

    USART_InitStructure.USART_StopBits =
        USART_StopBits_1;

    USART_InitStructure.USART_Parity =
        USART_Parity_No;

    USART_InitStructure.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;

    USART_InitStructure.USART_Mode =
        USART_Mode_Rx |
        USART_Mode_Tx;

    USART_Init(
        USART1,
        &USART_InitStructure
    );

    NVIC_InitStructure.NVIC_IRQChannel =
        USART1_IRQn;

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =
        0;

    NVIC_InitStructure.NVIC_IRQChannelSubPriority =
        0;

    NVIC_InitStructure.NVIC_IRQChannelCmd =
        ENABLE;

    NVIC_Init(
        &NVIC_InitStructure
    );

    USART_ITConfig(
        USART1,
        USART_IT_RXNE,
        ENABLE
    );

    USART_Cmd(
        USART1,
        ENABLE
    );
}

void USART1_SendChar(char c)
{
    while (
        USART_GetFlagStatus(
            USART1,
            USART_FLAG_TXE
        ) == RESET
    );

    USART_SendData(
        USART1,
        (uint16_t)c
    );
}

void USART1_SendString(const char *str)
{
    while (*str)
    {
        USART1_SendChar(*str);
        str++;
    }
}

void Set_PWM(uint8_t percent)
{
    uint16_t compare;

    if (percent > 100)
    {
        percent = 100;
    }

    compare =
        ((uint32_t)1000 * percent) / 100;

    if (compare > 999)
    {
        compare = 999;
    }

    TIM_SetCompare1(
        TIM2,
        compare
    );
}

void Process_Command(char *cmd)
{
    char number[8];

    if (strcmp(cmd, "ON") == 0)
    {
        led_on = 1;

        Set_PWM(pwm_percent);

        USART1_SendString(
            "LED ON - PWM="
        );

        sprintf(
            number,
            "%d",
            pwm_percent
        );

        USART1_SendString(number);
        USART1_SendString("%\r\n");
    }

    else if (strcmp(cmd, "OFF") == 0)
    {
        led_on = 0;

        Set_PWM(0);

        USART1_SendString(
            "LED OFF - PWM saved="
        );

        sprintf(
            number,
            "%d",
            pwm_percent
        );

        USART1_SendString(number);
        USART1_SendString("%\r\n");
    }

    else if (strcmp(cmd, "Status") == 0)
    {
        USART1_SendString(
            "STATUS: LED="
        );

        if (led_on)
        {
            USART1_SendString("ON");
        }
        else
        {
            USART1_SendString("OFF");
        }

        USART1_SendString(" PWM=");

        sprintf(
            number,
            "%d",
            pwm_percent
        );

        USART1_SendString(number);
        USART1_SendString("%\r\n");
    }

    else if (
        strncmp(
            cmd,
            "PWM:",
            4
        ) == 0
    )
    {
        char temp[16];
        char *percent_sign;
        int value;

        strcpy(
            temp,
            cmd + 4
        );

        percent_sign =
            strchr(temp, '%');

        if (percent_sign != NULL)
        {
            *percent_sign = '\0';
        }

        value = atoi(temp);

        if (
            value < 0 ||
            value > 100
        )
        {
            USART1_SendString(
                "ERROR: PWM must be 0-100%\r\n"
            );

            return;
        }

        pwm_percent =
            (uint8_t)value;

        if (led_on)
        {
            Set_PWM(pwm_percent);
        }
        else
        {
            Set_PWM(0);
        }

        USART1_SendString(
            "PWM SET="
        );

        sprintf(
            number,
            "%d",
            pwm_percent
        );

        USART1_SendString(number);
        USART1_SendString("%\r\n");
    }

    else
    {
        USART1_SendString(
            "ERROR: Unknown command\r\n"
        );
    }
}