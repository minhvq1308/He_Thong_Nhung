#include "main.h"
#include <stdint.h>
#include <string.h>

extern volatile char rx_buffer[64];
extern volatile uint8_t rx_index;

extern volatile char command[64];
extern volatile uint8_t command_ready;

void USART1_IRQHandler(void)
{
    if (
        USART_GetITStatus(
            USART1,
            USART_IT_RXNE
        ) != RESET
    )
    {
        char c;

        c = (char)USART_ReceiveData(USART1);

        if (c == '!')
        {
            rx_buffer[rx_index] = '\0';

            if (command_ready == 0)
            {
                strcpy(
                    (char *)command,
                    (const char *)rx_buffer
                );

                command_ready = 1;
            }

            rx_index = 0;
        }

        else if (
            c != '\r' &&
            c != '\n'
        )
        {
            if (rx_index < 63)
            {
                rx_buffer[rx_index] = c;
                rx_index++;
            }
            else
            {
                rx_index = 0;
            }
        }
    }
}