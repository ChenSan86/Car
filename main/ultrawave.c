#include <stdio.h>
#include "wiringx.h"
#include <unistd.h>
#include <sys/time.h>
#include "ultrawave.h"


void initUltr()
{
    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        wiringXGC();
        return -1;
    }
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
}
long getMicros()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}
double getDistance(){
    double dis[5] = {0};
    for(int i = 0;i<5;i++){
    digitalWrite(TRIG, 0);
    usleep(2);
    digitalWrite(TRIG, 1);
    usleep(10);
    digitalWrite(TRIG, 0);

    // 等待 Echo 变高
    while (digitalRead(ECHO) == 0)
        ;
    long start = getMicros();

    // 等待 Echo 变低
    while (digitalRead(ECHO) == 1)
        ;
    long end = getMicros();

    // 计算距离（单位：cm）
    long duration = end - start;
    double distance = duration / 58.0;
    dis[i] = distance;
    }
    double sum = 0;
    for(int i = 0;i<5;i++){
        sum += dis[i];
    }
    return sum/5;

}