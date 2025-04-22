#ifndef NEOPIXEL_DUO_H
#define NEOPIXEL_DUO_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wiringx.h>

// 颜色顺序定义（与Adafruit库保持一致）
#define NEO_RGB ((0 << 6) | (0 << 4) | (1 << 2) | (2))
#define NEO_GRB ((1 << 6) | (1 << 4) | (0 << 2) | (2))
#define NEO_KHZ800 0x0000

typedef struct
{
    uint16_t numLEDs;
    uint8_t pin;
    uint8_t brightness;
    uint8_t *pixels;
    uint8_t rOffset, gOffset, bOffset;
} NeoPixel;

// 初始化函数
void neoPixelInit(NeoPixel *strip, uint16_t n, uint8_t p, uint8_t t);
void neoPixelBegin(NeoPixel *strip);
void neoPixelShow(NeoPixel *strip);
void neoPixelSetColor(NeoPixel *strip, uint16_t n, uint8_t r, uint8_t g, uint8_t b);
void neoPixelClear(NeoPixel *strip);
uint32_t neoPixelColor(uint8_t r, uint8_t g, uint8_t b);

#endif