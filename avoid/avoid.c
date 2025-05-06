#include <unistd.h>
#include <wiringx.h>
#include <stdio.h>
#define PIN 18
int value = 0;
int main()
{
    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        wiringXGC();
        puts("wiringXSetup failed");
        return -1;
    }

    if (wiringXValidGPIO(PIN) != 0)
    {
        printf("Invalid GPIO %d\n", PIN);
    }
    pinMode(PIN, PINMODE_INPUT);
    while (1)
    {
        value = digitalRead(PIN);
        if (value)
        {
            puts("not find");
        }
        else
        {
            puts("find");
        }
        delayMicroseconds(1000000);
    }
    wiringXGC();
    return 0;
}