
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_i2c.h"

#define DS3231_ADDR 0x68


/* =========================================================
 * UART1
 * PA9  -> TX
 * PA10 -> RX
 * 9600 baud
 * ========================================================= */

void USART1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_USART1,
        ENABLE
    );

    /* PA9 - TX */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* PA10 - RX */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode =
        USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &USART_InitStructure);

    USART_Cmd(USART1, ENABLE);
}


/* =========================================================
 * Gửi 1 ký tự UART
 * ========================================================= */

void UART_SendChar(char c)
{
    while (USART_GetFlagStatus(
        USART1,
        USART_FLAG_TXE
    ) == RESET);

    USART_SendData(USART1, c);
}


/* =========================================================
 * Gửi chuỗi UART
 * ========================================================= */

void UART_SendString(char *str)
{
    while (*str)
    {
        UART_SendChar(*str);
        str++;
    }
}


/* =========================================================
 * I2C1
 *
 * PB6 -> SCL
 * PB7 -> SDA
 * 100 kHz
 * ========================================================= */

void I2C1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;

    /* Clock GPIOB */
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOB,
        ENABLE
    );

    /* Clock I2C1 */
    RCC_APB1PeriphClockCmd(
        RCC_APB1Periph_I2C1,
        ENABLE
    );

    /* PB6 = SCL
       PB7 = SDA */
    GPIO_InitStructure.GPIO_Pin =
        GPIO_Pin_6 | GPIO_Pin_7;

    GPIO_InitStructure.GPIO_Mode =
        GPIO_Mode_AF_OD;

    GPIO_InitStructure.GPIO_Speed =
        GPIO_Speed_50MHz;

    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* I2C configuration */
    I2C_InitStructure.I2C_ClockSpeed = 100000;

    I2C_InitStructure.I2C_Mode =
        I2C_Mode_I2C;

    I2C_InitStructure.I2C_DutyCycle =
        I2C_DutyCycle_2;

    I2C_InitStructure.I2C_OwnAddress1 =
        0x00;

    I2C_InitStructure.I2C_Ack =
        I2C_Ack_Enable;

    I2C_InitStructure.I2C_AcknowledgedAddress =
        I2C_AcknowledgedAddress_7bit;

    I2C_Init(I2C1, &I2C_InitStructure);

    I2C_Cmd(I2C1, ENABLE);
}


/* =========================================================
 * Đọc nhiều byte từ DS3231
 * ========================================================= */

void I2C_ReadMultiReg(
    uint8_t devAddr,
    uint8_t regAddr,
    uint8_t *pBuffer,
    uint16_t length
)
{
    /* START */
    I2C_GenerateSTART(I2C1, ENABLE);

    while (!I2C_CheckEvent(
        I2C1,
        I2C_EVENT_MASTER_MODE_SELECT
    ));

    /* Device address + WRITE */
    I2C_Send7bitAddress(
        I2C1,
        devAddr << 1,
        I2C_Direction_Transmitter
    );

    while (!I2C_CheckEvent(
        I2C1,
        I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED
    ));

    /* Gửi địa chỉ thanh ghi */
    I2C_SendData(I2C1, regAddr);

    while (!I2C_CheckEvent(
        I2C1,
        I2C_EVENT_MASTER_BYTE_TRANSMITTED
    ));

    /* REPEATED START */
    I2C_GenerateSTART(I2C1, ENABLE);

    while (!I2C_CheckEvent(
        I2C1,
        I2C_EVENT_MASTER_MODE_SELECT
    ));

    /* Device address + READ */
    I2C_Send7bitAddress(
        I2C1,
        devAddr << 1,
        I2C_Direction_Receiver
    );

    while (!I2C_CheckEvent(
        I2C1,
        I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED
    ));

    /* Đọc dữ liệu */
    while (length)
    {
        if (length == 1)
        {
            /* Byte cuối -> NACK */
            I2C_AcknowledgeConfig(
                I2C1,
                DISABLE
            );

            /* STOP */
            I2C_GenerateSTOP(
                I2C1,
                ENABLE
            );
        }

        while (!I2C_CheckEvent(
            I2C1,
            I2C_EVENT_MASTER_BYTE_RECEIVED
        ));

        *pBuffer = I2C_ReceiveData(I2C1);

        pBuffer++;
        length--;
    }

    /* Bật ACK lại */
    I2C_AcknowledgeConfig(
        I2C1,
        ENABLE
    );
}


/* =========================================================
 * BCD -> Decimal
 *
 * Ví dụ:
 * 0x25 -> 25
 * 0x59 -> 59
 * ========================================================= */

uint8_t BCD_To_Decimal(uint8_t bcd)
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}


/* =========================================================
 * Đọc thời gian DS3231
 *
 * Register:
 *
 * 0x00 -> Seconds
 * 0x01 -> Minutes
 * 0x02 -> Hours
 * 0x03 -> Day
 * 0x04 -> Date
 * 0x05 -> Month
 * 0x06 -> Year
 * ========================================================= */

void DS3231_ReadTime(
    uint8_t *second,
    uint8_t *minute,
    uint8_t *hour,
    uint8_t *day,
    uint8_t *date,
    uint8_t *month,
    uint8_t *year
)
{
    uint8_t data[7];

    I2C_ReadMultiReg(
        DS3231_ADDR,
        0x00,
        data,
        7
    );

    /* Seconds */
    *second = BCD_To_Decimal(
        data[0] & 0x7F
    );

    /* Minutes */
    *minute = BCD_To_Decimal(
        data[1] & 0x7F
    );

    /* Hours */
    *hour = BCD_To_Decimal(
        data[2] & 0x3F
    );

    /* Day of week */
    *day = data[3];

    /* Date */
    *date = BCD_To_Decimal(
        data[4] & 0x3F
    );

    /* Month */
    *month = BCD_To_Decimal(
        data[5] & 0x1F
    );

    /* Year */
    *year = BCD_To_Decimal(
        data[6]
    );
}


/* =========================================================
 * Delay ms
 * SYSCLK = 72 MHz
 * ========================================================= */

void Delay_ms(volatile uint32_t ms)
{
    while (ms--)
    {
        volatile uint32_t i;

        for (i = 0; i < 7200; i++)
        {
            __asm volatile ("nop");
        }
    }
}


/* =========================================================
 * In số có 2 chữ số
 *
 * Ví dụ:
 * 5  -> 05
 * 25 -> 25
 * ========================================================= */

void PrintNumber(uint8_t number)
{
    UART_SendChar(
        (number / 10) + '0'
    );

    UART_SendChar(
        (number % 10) + '0'
    );
}


/* =========================================================
 * MAIN
 * ========================================================= */

int main(void)
{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint8_t year;

    /* Khởi tạo clock */
    SystemInit();

    /* UART */
    USART1_Init();

    /* I2C */
    I2C1_Init();

    UART_SendString("\r\n");
    UART_SendString("STM32F103 + DS3231\r\n");
    UART_SendString("====================\r\n");

    while (1)
    {
        /* Đọc thời gian từ DS3231 */
        DS3231_ReadTime(
            &second,
            &minute,
            &hour,
            &day,
            &date,
            &month,
            &year
        );

        /* In giờ */
        UART_SendString("Time: ");

        PrintNumber(hour);
        UART_SendChar(':');

        PrintNumber(minute);
        UART_SendChar(':');

        PrintNumber(second);

        /* In ngày */
        UART_SendString("  ");

        PrintNumber(date);
        UART_SendChar('/');

        PrintNumber(month);

        UART_SendString("/20");

        PrintNumber(year);

        UART_SendString("\r\n");

        /* 1 giây */
        Delay_ms(1000);
    }
}

