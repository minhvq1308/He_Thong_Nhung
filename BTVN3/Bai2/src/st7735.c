#include "st7735.h"

/* ============================================================
 * Chân kết nối STM32F103C8T6 -> ST7735
 *
 * PA5 -> SCK
 * PA7 -> MOSI
 * PA4 -> CS
 * PB0 -> DC
 * PB1 -> RST
 * ============================================================ */

#define TFT_CS_PORT     GPIOA
#define TFT_CS_PIN      GPIO_Pin_4

#define TFT_DC_PORT     GPIOB
#define TFT_DC_PIN      GPIO_Pin_0

#define TFT_RST_PORT    GPIOB
#define TFT_RST_PIN     GPIO_Pin_1


/* ============================================================
 * Các lệnh ST7735
 * ============================================================ */

#define CMD_SWRESET     0x01
#define CMD_SLPOUT      0x11
#define CMD_COLMOD      0x3A
#define CMD_MADCTL      0x36
#define CMD_CASET       0x2A
#define CMD_RASET       0x2B
#define CMD_RAMWR       0x2C
#define CMD_DISPON      0x29


/* ============================================================
 * Delay đơn giản
 *
 * STM32F103C8T6 chạy khoảng 72 MHz.
 * Giá trị này chỉ dùng cho bài test cơ bản.
 * ============================================================ */

static void DelayMs(uint32_t ms)
{
    uint32_t i;
    uint32_t j;

    for (i = 0; i < ms; i++)
    {
        for (j = 0; j < 8000; j++)
        {
            __NOP();
        }
    }
}


/* ============================================================
 * Điều khiển CS
 * ============================================================ */

static void CS_Low(void)
{
    GPIO_ResetBits(TFT_CS_PORT, TFT_CS_PIN);
}

static void CS_High(void)
{
    GPIO_SetBits(TFT_CS_PORT, TFT_CS_PIN);
}


/* ============================================================
 * Điều khiển DC
 *
 * DC = 0 -> Command
 * DC = 1 -> Data
 * ============================================================ */

static void DC_Command(void)
{
    GPIO_ResetBits(TFT_DC_PORT, TFT_DC_PIN);
}

static void DC_Data(void)
{
    GPIO_SetBits(TFT_DC_PORT, TFT_DC_PIN);
}


/* ============================================================
 * Reset màn hình
 * ============================================================ */

static void TFT_Reset(void)
{
    GPIO_ResetBits(TFT_RST_PORT, TFT_RST_PIN);

    DelayMs(20);

    GPIO_SetBits(TFT_RST_PORT, TFT_RST_PIN);

    DelayMs(150);
}


/* ============================================================
 * Khởi tạo SPI1 + GPIO
 * ============================================================ */

static void SPI1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;


    /* --------------------------------------------------------
     * Enable clock:
     *
     * GPIOA
     * GPIOB
     * AFIO
     * SPI1
     * -------------------------------------------------------- */

    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_GPIOB |
        RCC_APB2Periph_AFIO |
        RCC_APB2Periph_SPI1,
        ENABLE
    );


    /* --------------------------------------------------------
     * PA5 = SPI1_SCK
     * PA7 = SPI1_MOSI
     *
     * Alternate Function Push-Pull
     * -------------------------------------------------------- */

    GPIO_InitStructure.GPIO_Pin =
        GPIO_Pin_5 |
        GPIO_Pin_7;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO_InitStructure);


    /* --------------------------------------------------------
     * PA4 = CS
     *
     * Output Push-Pull
     * -------------------------------------------------------- */

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO_InitStructure);


    /* --------------------------------------------------------
     * PB0 = DC
     * PB1 = RST
     * -------------------------------------------------------- */

    GPIO_InitStructure.GPIO_Pin =
        GPIO_Pin_0 |
        GPIO_Pin_1;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOB, &GPIO_InitStructure);


    /* --------------------------------------------------------
     * Trạng thái ban đầu
     * -------------------------------------------------------- */

    CS_High();

    DC_Data();

    GPIO_SetBits(TFT_RST_PORT, TFT_RST_PIN);


    /* ========================================================
     * Cấu hình SPI1
     * ======================================================== */

    SPI_InitStructure.SPI_Direction =
        SPI_Direction_2Lines_FullDuplex;

    SPI_InitStructure.SPI_Mode =
        SPI_Mode_Master;

    SPI_InitStructure.SPI_DataSize =
        SPI_DataSize_8b;

    SPI_InitStructure.SPI_CPOL =
        SPI_CPOL_Low;

    SPI_InitStructure.SPI_CPHA =
        SPI_CPHA_1Edge;

    SPI_InitStructure.SPI_NSS =
        SPI_NSS_Soft;

    SPI_InitStructure.SPI_BaudRatePrescaler =
        SPI_BaudRatePrescaler_4;

    SPI_InitStructure.SPI_FirstBit =
        SPI_FirstBit_MSB;

    SPI_InitStructure.SPI_CRCPolynomial =
        7;

    SPI_Init(SPI1, &SPI_InitStructure);


    /* Enable SPI1 */

    SPI_Cmd(SPI1, ENABLE);
}


/* ============================================================
 * Gửi 1 byte qua SPI1
 * ============================================================ */

static void SPI_SendByte(uint8_t data)
{
    /* Chờ TX buffer rỗng */

    while (
        SPI_I2S_GetFlagStatus(
            SPI1,
            SPI_I2S_FLAG_TXE
        ) == RESET
    )
    {
    }


    /* Gửi dữ liệu */

    SPI_I2S_SendData(SPI1, data);


    /* Chờ nhận xong */

    while (
        SPI_I2S_GetFlagStatus(
            SPI1,
            SPI_I2S_FLAG_RXNE
        ) == RESET
    )
    {
    }


    /* Đọc dữ liệu nhận để xóa cờ RXNE */

    SPI_I2S_ReceiveData(SPI1);
}


/* ============================================================
 * Gửi command
 * ============================================================ */

static void ST7735_WriteCommand(uint8_t command)
{
    DC_Command();

    SPI_SendByte(command);
}


/* ============================================================
 * Gửi data
 * ============================================================ */

static void ST7735_WriteData(uint8_t data)
{
    DC_Data();

    SPI_SendByte(data);
}


/* ============================================================
 * Gửi màu RGB565
 *
 * RGB565 = 16 bit
 *
 * Byte cao gửi trước
 * Byte thấp gửi sau
 * ============================================================ */

static void ST7735_WriteColor(uint16_t color)
{
    DC_Data();

    SPI_SendByte((uint8_t)(color >> 8));

    SPI_SendByte((uint8_t)(color & 0xFF));
}


/* ============================================================
 * Thiết lập vùng ghi màn hình
 * ============================================================ */

static void ST7735_SetWindow(
    uint8_t x0,
    uint8_t y0,
    uint8_t x1,
    uint8_t y1
)
{
    /* --------------------------------------------------------
     * Column Address Set
     * -------------------------------------------------------- */

    ST7735_WriteCommand(CMD_CASET);

    ST7735_WriteData(0x00);
    ST7735_WriteData(x0);

    ST7735_WriteData(0x00);
    ST7735_WriteData(x1);


    /* --------------------------------------------------------
     * Row Address Set
     * -------------------------------------------------------- */

    ST7735_WriteCommand(CMD_RASET);

    ST7735_WriteData(0x00);
    ST7735_WriteData(y0);

    ST7735_WriteData(0x00);
    ST7735_WriteData(y1);


    /* --------------------------------------------------------
     * Bắt đầu ghi RAM
     * -------------------------------------------------------- */

    ST7735_WriteCommand(CMD_RAMWR);
}


/* ============================================================
 * Khởi tạo ST7735
 * ============================================================ */

void ST7735_Init(void)
{
    /* Khởi tạo SPI + GPIO */

    SPI1_Init();


    /* Chọn TFT */

    CS_Low();


    /* Hardware reset */

    TFT_Reset();


    /* Software reset */

    ST7735_WriteCommand(CMD_SWRESET);

    DelayMs(150);


    /* Thoát sleep */

    ST7735_WriteCommand(CMD_SLPOUT);

    DelayMs(150);


    /* --------------------------------------------------------
     * Pixel format
     *
     * 0x05 = 16-bit RGB565
     * -------------------------------------------------------- */

    ST7735_WriteCommand(CMD_COLMOD);

    ST7735_WriteData(0x05);

    DelayMs(10);


    /* Memory Access Control */

    ST7735_WriteCommand(CMD_MADCTL);

    ST7735_WriteData(0x00);


    /* Bật màn hình */

    ST7735_WriteCommand(CMD_DISPON);

    DelayMs(100);


    /* Bỏ chọn TFT */

    CS_High();
}


/* ============================================================
 * Tô toàn bộ màn hình
 *
 * ST7735 128 x 160
 * = 20480 pixel
 *
 * Mỗi pixel = 2 byte RGB565
 * ============================================================ */

void ST7735_FillScreen(uint16_t color)
{
    uint32_t i;


    CS_Low();


    /* Toàn bộ vùng màn hình */

    ST7735_SetWindow(
        0,
        0,
        127,
        159
    );


    /* Gửi màu cho từng pixel */

    for (i = 0; i < 128UL * 160UL; i++)
    {
        ST7735_WriteColor(color);
    }


    CS_High();
}