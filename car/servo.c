#include "shin.h"
#include "servo.h"
int initServo(){
    wiringXPWMSetPeriod(PWM_SREVO, 20000000); // 20ms周期
    wiringXPWMSetPolarity(PWM_SREVO, 0);
    wiringXPWMEnable(PWM_SREVO, 1);
    center();
    return 0;
}
void turnRight()
{
    wiringXPWMSetDuty(PWM_SREVO, 50000);
    delayMicroseconds(1000000);
}
void turnLeft()
{
    wiringXPWMSetDuty(PWM_SREVO, 3000000);
    delayMicroseconds(1000000);
}
void center()
{
    wiringXPWMSetDuty(PWM_SREVO, 1500000);
    delayMicroseconds(1000000);
}
void turn(int angle)
{
    int duty = (3000000 - angle * 16666);
    wiringXPWMSetDuty(PWM_SREVO, duty);
    delayMicroseconds(1000000);
}
