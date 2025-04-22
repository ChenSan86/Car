#include "rgbh.h"
#include <stdio.h>

#define PIN 12 // GPIO12
#define MAX_LED 14

int main()
{
    NeoPixel strip;

    // 初始化 (使用NEO_RGB颜色顺序)
    neoPixelInit(&strip, MAX_LED, PIN, NEO_RGB);
    neoPixelBegin(&strip);

    // 点亮第一个LED为绿色
    neoPixelSetColor(&strip, 0, 0, 255, 0);
    neoPixelShow(&strip);

    // 保持状态
    while (1)
    {
        sleep(1);
    }

    return 0;
}