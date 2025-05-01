#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiringx.h>
#include <stdint.h>

// 硬件参数
#define SPI_CHANNEL 0     // Duo的SPI通道
#define SPI_SPEED 6000000 // 6MHz（每个bit≈166.67ns）
#define LED_COUNT 3       // LED数量
#define RESET_BYTES 50    // 复位信号长度（50字节≈50us）

// WS2812B编码规则（每个bit对应1字节SPI数据）
#define BIT_0 0xC0 // 0b11000000（高电平2 bits）
#define BIT_1 0xF8 // 0b11111000（高电平5 bits）

// 将RGB颜色编码为SPI数据流（GRB顺序）
static void encode_led_color(uint8_t r, uint8_t g, uint8_t b, uint8_t *buffer)
{
    for (int i = 7; i >= 0; i--)
    { // 绿色（高位先传）
        *buffer++ = (g & (1 << i)) ? BIT_1 : BIT_0;
    }
    for (int i = 7; i >= 0; i--)
    { // 红色
        *buffer++ = (r & (1 << i)) ? BIT_1 : BIT_0;
    }
    for (int i = 7; i >= 0; i--)
    { // 蓝色
        *buffer++ = (b & (1 << i)) ? BIT_1 : BIT_0;
    }
}

int main()
{
    // 初始化SPI
    if (wiringXSPISetup(SPI_CHANNEL, SPI_SPEED) < 0)
    {
        printf("[错误] SPI初始化失败\n");
        return 1;
    }

    // 定义LED颜色（示例：红、绿、蓝）
    uint8_t leds[LED_COUNT][3] = {
        {255, 0, 0}, // 红色
        {0, 255, 0}, // 绿色
        {0, 0, 255}  // 蓝色
    };

    // 创建SPI数据缓冲区（每个LED24字节 + 复位信号）
    int data_size = LED_COUNT * 24 + RESET_BYTES;
    uint8_t *tx_buffer = (uint8_t *)malloc(data_size);
    if (!tx_buffer)
    {
        fprintf(stderr, "[错误] 内存分配失败\n");
        return 1;
    }
    memset(tx_buffer, 0, data_size);

    // 编码所有LED颜色数据
    uint8_t *ptr = tx_buffer;
    for (int i = 0; i < LED_COUNT; i++)
    {
        encode_led_color(leds[i][0], leds[i][1], leds[i][2], ptr);
        ptr += 24;
    }

    // 发送数据（包含复位信号）
    if (wiringXSPIDataRW(SPI_CHANNEL, tx_buffer, data_size) < 0)
    {
        fprintf(stderr, "[错误] SPI数据传输失败\n");
        free(tx_buffer);
        return 1;
    }

    // 清理资源
    free(tx_buffer);
    printf("WS2812B灯带控制完成！\n");
    return 0;
}