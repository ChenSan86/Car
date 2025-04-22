#include <stdio.h>
#include <unistd.h>
#include <wiringx.h>

// 引脚定义
#define IN1 11
#define IN2 12
#define ENA 4 // PWM控制引脚

// PWM参数
#define PWM_PERIOD 2000000     // 2ms周期（比标准20ms快10倍！）
#define PWM_MIN_DUTY 50000     // 0.05ms（最小有效占空比）
#define PWM_MAX_DUTY 500000    // 0.5ms（100%速度）
#define DUTY_25_PERCENT 150000 // 25%速度（实测优化值）
#define RUN_DURATION 5000000   // 5秒运行时间

// 设置电机方向
void setDirection(int forward)
{
    digitalWrite(IN1, forward ? 1 : 0);
    digitalWrite(IN2, forward ? 0 : 1);
}

// 设置固定速度运行指定时间
void runMotor(int direction, long duty, long duration_us)
{
    setDirection(direction);
    wiringXPWMSetDuty(ENA, duty);
    printf("[Motor] %s | Duty: %ld ns (%.0f%%)\n",
           direction ? "Forward " : "Backward",
           duty,
           (duty - PWM_MIN_DUTY) * 100.0 / (PWM_MAX_DUTY - PWM_MIN_DUTY));
    usleep(duration_us);
}

int main()
{
    // 初始化检查
    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        wiringXGC();
        return -1;
    }

    // 引脚模式设置
    pinMode(IN1, PINMODE_OUTPUT);
    pinMode(IN2, PINMODE_OUTPUT);

    // PWM配置
    wiringXPWMSetPeriod(ENA, PWM_PERIOD);
    wiringXPWMSetPolarity(ENA, 0);
    wiringXPWMEnable(ENA, 1);

    printf("=== Motor Speed Test ===\n");

    // 正向测试序列
    runMotor(1, DUTY_25_PERCENT, RUN_DURATION);  // 正向25%速度 5秒
    runMotor(1, PWM_MAX_DUTY, RUN_DURATION); // 正向100%速度 5秒

    // 反向测试序列
    runMotor(0, DUTY_25_PERCENT, RUN_DURATION);  // 反向25%速度 5秒
    runMotor(0, PWM_MAX_DUTY, RUN_DURATION); // 反向100%速度 5秒

    // 完全停止
    wiringXPWMSetDuty(ENA, 0);
    digitalWrite(IN1, 0);
    digitalWrite(IN2, 0);
    printf("Motor stopped.\n");

    return 0;
}