#include <stdio.h>
#include <unistd.h>
#include <wiringx.h>

#define IN1 11
#define IN2 12
#define ENA 4 // PWM 控制电机速度

void setDirection(int forward)
{
    if (forward)
    {
        digitalWrite(IN1, 1);
        digitalWrite(IN2, 0);
    }
    else
    {
        digitalWrite(IN1, 0);
        digitalWrite(IN2, 1);
    }
}

void gradualSpeed(int direction, int up)
{
    setDirection(direction);
    for (int duty = (up ? 100000 : 1000000); (up ? duty <= 1000000 : duty >= 100000); duty += (up ? 100000 : -100000))
    {
        wiringXPWMSetDuty(ENA, duty);
        printf("PWM duty: %d\n", duty);
        usleep(500000); // 每步 0.5 秒，总计 5 秒
    }
}

int main()
{
    if (wiringXSetup("milkv_duo", NULL) == -1)
        return -1;

    pinMode(IN1, PINMODE_OUTPUT);
    pinMode(IN2, PINMODE_OUTPUT);

    wiringXPWMSetPeriod(ENA, 20000000); // 20ms PWM周期
    wiringXPWMSetPolarity(ENA, 0);
    wiringXPWMEnable(ENA, 1);

    // 正向加速
    gradualSpeed(1, 1);
    // 正向减速
    gradualSpeed(1, 0);
    // 反向加速
    gradualSpeed(0, 1);
    // 反向减速
    gradualSpeed(0, 0);

    // 停止
    wiringXPWMSetDuty(ENA, 0);
    digitalWrite(IN1, 0);
    digitalWrite(IN2, 0);

    return 0;
}