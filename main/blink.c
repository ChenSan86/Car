#include <unistd.h>
#include <wiringx.h>
#include <stdio.h>
#include "blink.h"
void blink(){
    int counter = 2;
    int LED = 25;
    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        wiringXGC();
        return;
    }
    if (wiringXValidGPIO(LED) != 0)
    {
        printf("Invalid GPIO %d\n", LED);
    }

    pinMode(LED, PINMODE_OUTPUT);

    while (counter--)
    {
        printf("Duo LED GPIO (wiringX) %d: High\n", LED);
        digitalWrite(LED, HIGH);
        sleep(1);
        printf("Duo LED GPIO (wiringX) %d: Low\n", LED);
        digitalWrite(LED, LOW);
        sleep(1);
    }
}