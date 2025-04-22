#include "rgbh.h"
#include <unistd.h>

// WS2812B 时序参数 (单位：纳秒)
#define T0H 400
#define T0L 850
#define T1H 800
#define T1L 450
#define RESET_DELAY 60

void neoPixelInit(NeoPixel *strip, uint16_t n, uint8_t p, uint8_t t)
{
    strip->numLEDs = n;
    strip->pin = p;
    strip->brightness = 255;

    // 解析颜色顺序
    strip->rOffset = (t >> 4) & 0x03;
    strip->gOffset = (t >> 2) & 0x03;
    strip->bOffset = t & 0x03;

    // 分配像素缓冲区
    strip->pixels = (uint8_t *)malloc(n * 3);
    memset(strip->pixels, 0, n * 3);
}

void neoPixelBegin(NeoPixel *strip)
{
    const char *platforms[] = {"milkv_duo", "milkv_duo256m", NULL};
    for (int i = 0; platforms[i]; i++)
    {
        if (wiringXSetup(platforms[i], NULL) == 0)
        {
            printf("Initialized for %s\n", platforms[i]);
            break;
        }
    }
    pinMode(strip->pin, OUTPUT);
}

static void sendByte(NeoPixel *strip, uint8_t byte)
{
    for (int i = 7; i >= 0; i--)
    {
        digitalWrite(strip->pin, HIGH);
        delayMicroseconds((byte >> i) & 0x01 ? T1H : T0H);
        digitalWrite(strip->pin, LOW);
        delayMicroseconds((byte >> i) & 0x01 ? T1L : T0L);
    }
}

void neoPixelShow(NeoPixel *strip)
{
    for (uint16_t i = 0; i < strip->numLEDs; i++)
    {
        sendByte(strip, strip->pixels[i * 3 + strip->gOffset]); // G
        sendByte(strip, strip->pixels[i * 3 + strip->rOffset]); // R
        sendByte(strip, strip->pixels[i * 3 + strip->bOffset]); // B
    }
    digitalWrite(strip->pin, LOW);
    usleep(RESET_DELAY);
}

void neoPixelSetColor(NeoPixel *strip, uint16_t n, uint8_t r, uint8_t g, uint8_t b)
{
    if (n < strip->numLEDs)
    {
        uint8_t *p = &strip->pixels[n * 3];
        p[strip->rOffset] = r;
        p[strip->gOffset] = g;
        p[strip->bOffset] = b;
    }
}

void neoPixelClear(NeoPixel *strip)
{
    memset(strip->pixels, 0, strip->numLEDs * 3);
}

uint32_t neoPixelColor(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}