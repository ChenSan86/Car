#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <wiringx.h>

#define TRIG 21
#define ECHO 20

// 获取当前时间（微秒）
long getMicros()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

int main()
{
    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        wiringXGC();
        return -1;
    }
    pinMode(TRIG, PINMODE_OUTPUT);
    pinMode(ECHO, PINMODE_INPUT);

    while (1)
    {
        digitalWrite(TRIG, 0);
        usleep(2);
        digitalWrite(TRIG, 1);
        usleep(10);
        digitalWrite(TRIG, 0);
        while (digitalRead(ECHO) == 0)
            ;
        long start = getMicros();
        while (digitalRead(ECHO) == 1)
            ;
        long end = getMicros();
        

        // 计算距离（单位：cm）
        long duration = end - start;
        float distance = duration / 58.0;

        printf("Distance: %.2f cm\n", distance);
        usleep(1000000); // 1秒测一次
    }

    return 0;
}