#include "shin.h"
#include "distance.h"
char *data;
long getMicroseconds()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}
int initDistance(){
    if (wiringXValidGPIO(TRIG) != 0)
    {
        printf("Invalid GPIO %d\n", EN1);
        return -1;
    }
    if (wiringXValidGPIO(ECHO) != 0)
    {
        printf("Invalid GPIO %d\n", EN1);
        return -1;
    }
    if (wiringXValidGPIO(EN1) != 0)
    {
        printf("Invalid GPIO %d\n", EN1);
        return -1;
    }
    if (wiringXValidGPIO(EN1) != 0)
    {
        printf("Invalid GPIO %d\n", EN1);
        return -1;
    }
    if (wiringXValidGPIO(RED_L) != 0)
    {
        printf("Invalid GPIO %d\n", EN1);
        return -1;
    }
    if (wiringXValidGPIO(RED_R) != 0)
    {
        printf("Invalid GPIO %d\n", EN1);
        return -1;
    }
    pinMode(TRIG, PINMODE_OUTPUT);
    pinMode(ECHO, PINMODE_INPUT);
    pinMode(RED_R, PINMODE_INPUT);
    pinMode(RED_L, PINMODE_INPUT);
    data = (char *)malloc(50);
    return 0;
};
char* getDistance(){
    digitalWrite(TRIG, LOW);
    usleep(2);
    digitalWrite(TRIG, HIGH);
    usleep(10);
    digitalWrite(TRIG, LOW);
    while (digitalRead(ECHO) == 0);
    long start = getMicros();
    while (digitalRead(ECHO) == 1);
    long end = getMicros();
    long duration = end - start;
    float distance = duration / 58.0;
    sprintf(data,"Distance: %.2f cm\n", distance);
    return data;
}
int getR(){
    return digitalRead(RED_R);
}
int getL(){
    return digitalRead(RED_L);
}