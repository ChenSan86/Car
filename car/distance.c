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
        printf("Invalid GPIO %d\n", TRIG);
        return -1;
    }
    if (wiringXValidGPIO(ECHO) != 0)
    {
        printf("Invalid GPIO %d\n", ECHO);
        return -1;
    }
    if (wiringXValidGPIO(RED_L) != 0)
    {
        printf("Invalid GPIO %d\n", RED_L);
        return -1;
    }
    if (wiringXValidGPIO(RED_R) != 0)
    {
        printf("Invalid GPIO %d\n", RED_R);
        return -1;
    }
    pinMode(TRIG, PINMODE_OUTPUT);
    pinMode(ECHO, PINMODE_INPUT);
    pinMode(RED_R, PINMODE_INPUT);
    pinMode(RED_L, PINMODE_INPUT);
    data = (char *)malloc(50);
    return 0;
};
float getDistance(){
    digitalWrite(TRIG, LOW);
    usleep(2);
    digitalWrite(TRIG, HIGH);
    usleep(10);
    digitalWrite(TRIG, LOW);
    while (digitalRead(ECHO) == 0);
    long start = getMicros();
    while (digitalRead(ECHO) == 1);
    long end = getMicroseconds();
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
    sprintf(data, ":L%dR%d", getR(), getL());
    return data;
}