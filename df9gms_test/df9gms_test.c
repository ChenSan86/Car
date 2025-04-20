#include <stdio.h>
#include <unistd.h>
#include <wiringx.h>

static int PWM_PIN = 4; // PWM5@GP4
// 左转500000
// 右转3000000
// 中间1500000

void turnLeft(int delay_ms){
    wiringXPWMSetDuty(PWM_PIN, 500000);
    printf("Duty: %ld ns\n", 500000);
    delayMicroseconds(delay_ms); // 转为微秒
}
void turnRight(int delay_ms){
    wiringXPWMSetDuty(PWM_PIN, 3000000);
    printf("Duty: %ld ns\n", 3000000);
    delayMicroseconds(delay_ms); // 转为微秒
}
void center(int delay_ms){
    wiringXPWMSetDuty(PWM_PIN, 1500000);
    printf("Duty: %ld ns\n", 1500000);
    delayMicroseconds(delay_ms); // 转为微秒
}

void moveServo(long duty, int delay_ms)
{
    wiringXPWMSetDuty(PWM_PIN, duty);
    printf("Duty: %ld ns\n", duty);
    delayMicroseconds(delay_ms * 1000); // 转为微秒
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

    // 初始化为中间位置
    moveServo(1500000, 1000); // 中间，停 1 秒

    // 左转
    turnLeft(500000); // 左转 1 秒

    center(500000); // 中间，停 1 秒
    // 右转
    turnRight(500000); // 右转 1 秒
    // 中间
    center(500000); // 中间，停 1 秒

    return 0;
}