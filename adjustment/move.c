#include <stdio.h>
#include "move.h"
#include <unistd.h>
#include <wiringx.h>
#include <math.h>

int speedr = 100, speedl = 100;
void initMove()
{

    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        wiringXGC();
        return;
    }
    wiringXPWMSetPeriod(PWM_1, PERIOD); // 500us
    wiringXPWMSetPolarity(PWM_1, 0);
    wiringXPWMEnable(PWM_1, 1);
    wiringXPWMSetPeriod(PWM_2, PERIOD); // 500us
    wiringXPWMSetPolarity(PWM_2, 0);
    wiringXPWMEnable(PWM_2, 1);
    if (wiringXValidGPIO(EN1) != 0)
    {
        printf("Invalid GPIO %d\n", EN1);
    }
    if (wiringXValidGPIO(EN2) != 0)
    {
        printf("Invalid GPIO %d\n", EN2);
    }
    if (wiringXValidGPIO(EN3) != 0)
    {
        printf("Invalid GPIO %d\n", EN3);
    }
    if (wiringXValidGPIO(EN4) != 0)
    {
        printf("Invalid GPIO %d\n", EN4);
    }

    pinMode(EN3, PINMODE_OUTPUT);
    pinMode(EN4, PINMODE_OUTPUT);
    pinMode(EN1, PINMODE_OUTPUT);
    pinMode(EN2, PINMODE_OUTPUT);
    digitalWrite(EN1, LOW);
    digitalWrite(EN2, LOW);
    digitalWrite(EN3, LOW);
    digitalWrite(EN4, LOW);
}
void Move(float l, float r)
{
    if (l > 0)
        digitalWrite(EN1, HIGH),digitalWrite(EN2, LOW);
    else if (l < 0)
        digitalWrite(EN1, LOW),digitalWrite(EN2, HIGH);
    else
        digitalWrite(EN1, LOW),digitalWrite(EN2, LOW);
    if (r > 0)
        digitalWrite(EN3, HIGH),digitalWrite(EN4, LOW);
    else if (r < 0)
        digitalWrite(EN3, LOW),digitalWrite(EN4, HIGH);
    else
        digitalWrite(EN3, LOW),digitalWrite(EN4, LOW);
    wiringXPWMSetDuty(PWM_2, (long)(speedr * fabs(r) > 100 ? 100 : speedr * fabs(r))*T);
    wiringXPWMSetDuty(PWM_1, (long)(speedl * fabs(l) > 100 ? 100 : speedl * fabs(l))*T);
    // printf("left = %f, right = %f\n", speedr * fabs(l) > 100 ? 100 : speedr * l, speedr * fabs(r) > 100 ? 100 : speedr * r);
}
void brake()
{
    Move(0, 0);
}
void forward()
{
    Move(1, 1);
}
void back()
{
    Move(-1, -1);
}
void left()
{
    Move(0, 1);
}
void right()
{
    Move(1, 0);
}
void rotationL()
{
    Move(-1, 1);
}
void rotationR()
{
    Move(1, -1);
}