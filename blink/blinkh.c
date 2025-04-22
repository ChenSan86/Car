#include <stdio.h>
#include "blinkh.h"
int ifReady(){
    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        wiringXGC();
        return 0;
    }

    if (wiringXValidGPIO(BLINK_PIN) != 0)
    {
        printf("Invalid GPIO %d\n", BLINK_PIN);
    }

    pinMode(BLINK_PIN, PINMODE_OUTPUT);
    return 1;
}
void blink(int count, int delay, int length){
    while (count > 0)
    {
        printf("Duo LED GPIO (wiringX) %d: High\n", BLINK_PIN);
        digitalWrite(BLINK_PIN, HIGH);
        sleep(length);
        printf("Duo LED GPIO (wiringX) %d: Low\n", BLINK_PIN);
        digitalWrite(BLINK_PIN, LOW);
        sleep(delay);
        count--;
    }
}
