#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "wiringx.h"
#include <string.h>
#include <unistd.h>


// LED 数量
#define LED_COUNT 8
#define SPI_CHANNEL 0
#define SPI_SPEED 2400000 // 2.4MHz SPI 时钟

// 编码 WS2812 数据到 SPI 格式（每个 bit 用 3 个 SPI bit 模拟）
uint8_t encodeBitToSPI(uint8_t bit)
{
    return bit ? 0b110 : 0b100; // 1: high-high-low, 0: high-low-low
}

// 将一个字节编码为 24 个 SPI bit
void encodeByte(uint8_t byte, uint8_t *out, int *bitPos)
{
    for (int i = 7; i >= 0; i--)
    {
        uint8_t encoded = encodeBitToSPI((byte >> i) & 1);
        for (int j = 2; j >= 0; j--)
        {
            int byteIndex = *bitPos / 8;
            int bitIndex = 7 - (*bitPos % 8);
            out[byteIndex] |= ((encoded >> j) & 1) << bitIndex;
            (*bitPos)++;
        }
    }
}

// 发送 WS2812 数据
void sendWS2812(uint8_t rgb[LED_COUNT][3])
{
    int totalBits = LED_COUNT * 24 * 3; // 每个灯珠 24 bit，3 字节
    int totalBytes = (totalBits + 7) / 8;
    uint8_t *spiData = calloc(totalBytes, 1); // 动态分配内存存储 SPI 数据
    int bitPos = 0;

    // 遍历所有 LED，将每个 LED 的 RGB 编码为 24-bit 数据
    for (int i = 0; i < LED_COUNT; i++)
    {
        encodeByte(rgb[i][1], spiData, &bitPos); // G
        encodeByte(rgb[i][0], spiData, &bitPos); // R
        encodeByte(rgb[i][2], spiData, &bitPos); // B
    }

    // 配置 SPI
    wiringXSPISetup(SPI_CHANNEL, SPI_SPEED);
    // 发送 SPI 数据
    wiringXSPIDataRW(SPI_CHANNEL, spiData, totalBytes);
    puts("send");
    delayMicroseconds(1000000000);//
    puts("send");

    // 释放内存
    free(spiData);
    usleep(80); // 等待 80us 用于 WS2812 的复位时间
}

int main()
{
    // 创建 RGB 数据（红色）
    uint8_t rgb[LED_COUNT][3];

    // 设置每个灯珠的颜色为红色
    for (int i = 0; i < LED_COUNT; i++)
    {
        rgb[i][0] = 255; // R
        rgb[i][1] = 0;   // G
        rgb[i][2] = 0;   // B
    }

    // 发送数据给 WS2812
    sendWS2812(rgb);

    return 0;
}