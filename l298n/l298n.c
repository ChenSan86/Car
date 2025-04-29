#include <stdio.h>
#include <wiringx.h>

// 引脚定义
#define IN1 11 // 电机方向控制1
#define IN2 12 // 电机方向控制2
#define ENA 4 // PWM调速引脚 (必须支持PWM)

// PWM参数 (单位:纳秒)
#define PWM_PERIOD 20000 // 20ms周期(500Hz)
#define PWM_MIN 1000     // 最小脉宽1ms
#define PWM_MAX 2000     // 最大脉宽2ms
#define PWM_STOP 1500    // 停止脉宽1.5ms

int main()
{
    // 初始化WiringX
    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        printf("WiringX初始化失败！\n");
        return -1;
    }

    // 设置引脚模式
    pinMode(IN1, PINMODE_OUTPUT);
    pinMode(IN2, PINMODE_OUTPUT);

    // 配置PWM
    wiringXPWMSetPeriod(ENA, PWM_PERIOD);
    wiringXPWMSetDuty(ENA, PWM_STOP); // 初始停止状态
    wiringXPWMEnable(ENA, 1);         // 启用PWM

    printf("L298N PWM调速测试开始\n");

    // 正转加速测试
    printf("正转加速...\n");
    digitalWrite(IN1, 1);
    digitalWrite(IN2, 0);

    for (int duty = PWM_MIN; duty <= PWM_MAX; duty += 100)
    {
        wiringXPWMSetDuty(ENA, duty);
        printf("Duty: %d ns\n", duty);
        delayMicroseconds(2000000); // 每100ms加速一次
    }
    delayMicroseconds(2000000); // 全速运行2秒
    //两秒等于多少微秒？


    // 减速停止
    printf("减速停止...\n");
    for (int duty = PWM_MAX; duty >= PWM_MIN; duty -= 100)
    {
        wiringXPWMSetDuty(ENA, duty);
        printf("Duty: %d ns\n", duty);
        delayMicroseconds(2000000);
    }

    // 反转测试
    printf("反转测试...\n");
    digitalWrite(IN1, 0);
    digitalWrite(IN2, 1);
    wiringXPWMSetDuty(ENA, PWM_MAX);
    delayMicroseconds(2000000);

    // 完全停止
    printf("停止电机\n");
    digitalWrite(IN1, 0);
    digitalWrite(IN2, 0);
    wiringXPWMSetDuty(ENA, PWM_STOP);

    wiringXGC(); // 清理资源
    return 0;
}