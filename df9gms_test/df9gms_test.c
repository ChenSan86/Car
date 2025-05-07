#include <stdio.h>
#include <unistd.h>
#include <wiringx.h>
static int PWM_PIN = 6; // PWM5@GP4
void turnRight(){
    wiringXPWMSetDuty(PWM_PIN, 50000);
}
void turnLeft(){
    wiringXPWMSetDuty(PWM_PIN, 3000000);
}
void center(){
    wiringXPWMSetDuty(PWM_PIN, 1500000);
}

void moveServo(long duty, int delay_ms)
{
    wiringXPWMSetDuty(PWM_PIN, duty);
}
//0 - 180度
//500000 3000000 1500000
void turn(int angle){
    int duty = (3000000 - angle * 16666);
    moveServo(duty, 1000);
}

int main()
{
    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        wiringXGC();
        return -1;
    }

    wiringXPWMSetPeriod(PWM_PIN, 20000000); // 20ms周期
    wiringXPWMSetPolarity(PWM_PIN, 0);
    wiringXPWMEnable(PWM_PIN, 1);


    // 左转
    turnLeft(500000); // 左转 1 秒

    center(500000); // 中间，停 1 秒
    // 右转
    turnRight(500000); // 右转 1 秒
    // 中间
    center(500000); // 中间，停 1 秒

    turn(30); // 转180度
    delayMicroseconds(1000000); // 1s
    turn(60); // 转90度
    delayMicroseconds(1000000); // 1s
    turn(90); // 转0度
    delayMicroseconds(1000000); // 1s
    turn(120); // 转-90度
    delayMicroseconds(1000000); // 1s
    turn(150); // 转-180度
    delayMicroseconds(1000000); // 1s
    turn(180); // 转-180度
    delayMicroseconds(1000000); // 1s

    center(500000); // 中间，停 1 秒
    wiringXGC();
    return 0;
}