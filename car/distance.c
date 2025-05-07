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

    return distance;
}
int getR(){
    return digitalRead(RED_R);
}
int getL(){
    return digitalRead(RED_L);
}
char* getRL(){
    sprintf(data_dis, ":L%dR%d", getR(), getL());
    return data_dis;
}