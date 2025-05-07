#include "shin.h"
#include "move.h"
int speedr = 100, speedl = 100;
int initMove()
{
    wiringXPWMSetPeriod(PWM_1, PERIOD); // 500us
    wiringXPWMSetPolarity(PWM_1, 0);
    wiringXPWMEnable(PWM_1, 1);
    wiringXPWMSetPeriod(PWM_2, PERIOD); // 500us
    wiringXPWMSetPolarity(PWM_2, 0);
    wiringXPWMEnable(PWM_2, 1);
    if (wiringXValidGPIO(EN1) != 0)
    {
        printf("Invalid GPIO %d\n", EN1);
        return -1;
    }
    if (wiringXValidGPIO(EN2) != 0)
    {
        printf("Invalid GPIO %d\n", EN2);
        return -1;
    }
    if (wiringXValidGPIO(EN3) != 0)
    {
        printf("Invalid GPIO %d\n", EN3);
        return -1;
    }
    if (wiringXValidGPIO(EN4) != 0)
    {
        printf("Invalid GPIO %d\n", EN4);
        return -1;
    }
    if (wiringXValidGPIO(WHISTLE)!= 0)
    {
        printf("Invalid GPIO %d\n", WHISTLE);
        return -1;
    }
    pinMode(EN3, PINMODE_OUTPUT);
    pinMode(EN4, PINMODE_OUTPUT);
    pinMode(EN1, PINMODE_OUTPUT);
    pinMode(EN2, PINMODE_OUTPUT);
    pinMode(WHISTLE, PINMODE_OUTPUT);
    digitalWrite(WHISTLE, HIGH);
    digitalWrite(EN1, LOW);
    digitalWrite(EN2, LOW);
    digitalWrite(EN3, LOW);
    digitalWrite(EN4, LOW);
    return 0;
}
void Move(float l, float r)
{
    if (l > 0)
        digitalWrite(EN1, HIGH), digitalWrite(EN2, LOW);
    else if (l < 0)
        digitalWrite(EN1, LOW), digitalWrite(EN2, HIGH);
    else
        digitalWrite(EN1, LOW), digitalWrite(EN2, LOW);
    if (r > 0)
        digitalWrite(EN3, HIGH), digitalWrite(EN4, LOW);
    else if (r < 0)
        digitalWrite(EN3, LOW), digitalWrite(EN4, HIGH);
    else
        digitalWrite(EN3, LOW), digitalWrite(EN4, LOW);
    wiringXPWMSetDuty(PWM_2, (long)(speedr * fabs(r) > 100 ? 100 : speedr * fabs(r)) * T);
    wiringXPWMSetDuty(PWM_1, (long)(speedl * fabs(l) > 100 ? 100 : speedl * fabs(l)) * T);
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
void whistle(){
    digitalWrite(WHISTLE, LOW);
    sleep(1);
    digitalWrite(WHISTLE, HIGH);
}
void Move_left_angle(int angle) // 左转
{
    Move(-1, 1);
    delayMicroseconds(angle * 550 / 90);
    Move(0, 0);
    delayMicroseconds(100);
}
void Move_right_angle(int angle) // 左转
{
    Move(1, -1);
    delayMicroseconds(angle * 550 / 90);
    Move(0, 0);
    delayMicroseconds(100);
}