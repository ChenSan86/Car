#ifndef BLINKH_H
#define BLINKH_H
#include <stdio.h>
#include <unistd.h>
#include <wiringx.h>
#define BLINK_PIN 25
int ifReady();
void blink(int count,int delay,int length);

#endif