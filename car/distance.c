#include "shin.h"
#include "distance.h"
#include <time.h>
static char datadis[50];
char* data_dis = datadis;
long getMicros()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

int initDistance(){

    pinMode(TRIG, PINMODE_OUTPUT);
    pinMode(ECHO, PINMODE_INPUT);
    pinMode(RED_R, PINMODE_INPUT);
    pinMode(RED_L, PINMODE_INPUT);
    return 0;
};
float getDistance(){
    // 发送10微秒的高电平触发信号
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    // 等待回波引脚变高
    while (digitalRead(ECHO) == LOW)
        ;
    long startTime = getMicros();

    // 等待回波引脚变低
    while (digitalRead(ECHO) == HIGH)
        ;
    long endTime = getMicros();

    // 计算时间差（微秒）
    long duration = endTime - startTime;

    // 计算距离（厘米）：声速340m/s = 0.034cm/μs，往返距离所以要除以2
    float distance = duration * 0.034 / 2;

    return distance;
}
int getR(){
    return digitalRead(RED_R);
}
int getL(){
    return digitalRead(RED_L);
}
char* getRL(){
    sprintf(data_dis, "AVOID:%d%d", getR(), getL());
    return data_dis;
}
