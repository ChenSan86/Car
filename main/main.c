#include "move.h"

// PID参数（需根据实际调试调整）
#define KP 10 // 比例系数
#define KI 0.06 // 积分系数
#define KD 0.05  // 微分系数
#define SENSOR_SPACING 1 // 传感器间距（单位：mm）

// 传感器位置权重（单位：mm）
static const float sensor_weights[8] = {
    -5.5 * SENSOR_SPACING, // 最左传感器
    -2.5 * SENSOR_SPACING,
    -1.5 * SENSOR_SPACING,
    -0.5 * SENSOR_SPACING,
    0.5 * SENSOR_SPACING,
    1.5 * SENSOR_SPACING,
    2.5 * SENSOR_SPACING,
    5.5 * SENSOR_SPACING // 最右传感器
};

// PID控制器状态变量
static float integral = 0;
static float last_error = 0;

// 基础速度（最大速度的百分比）
#define BASE_SPEED 70

void pid_tracking()
{
    // 1. 获取传感器数据
    getTrace();
    //打印数组
    for (int i = 0; i < 9; i++)
    {
        printf("%d \t", trace[i]);
    }
    puts("");
    if(1){

    // 2. 计算当前位置偏差（加权平均法）
    float numerator = 0;
    float denominator = 0;

    for (int i = 0; i < 8; i++)
    {
        if (trace[i + 1])
        { // 只计算检测到黑线的传感器
            numerator += sensor_weights[i];
            denominator += 1;
        }
    }

    // 若无传感器检测到黑线，保持上次状态或停止
    if (denominator == 0)
    {
        motor_run(0, 0); // 安全停止
        return;
    }

    float position = numerator / denominator;

    // 3. PID计算
    float error = -position; // 误差（负号是因为需要反向修正）
    integral += error;
    float derivative = error - last_error;

    // PID输出（控制量）
    float output = KP * error + KI * integral + KD * derivative;
    last_error = error;

    // 4. 电机控制（差速转向）
    float left_speed = BASE_SPEED - output;
    float right_speed = BASE_SPEED + output;
    // 限幅处理（确保速度在-100到100之间）
    left_speed = (left_speed > 100) ? 100 : (left_speed < -100) ? -100
                                                               : left_speed;
    right_speed = (right_speed > 100) ? 100 : (right_speed < -100) ? -100
                                                                   : right_speed;

    // 5. 执行电机控制
    motor_run((long)left_speed, (long)right_speed);
    printf("left_speed: %f, right_speed: %f\n", (long)left_speed, (long)right_speed);
    delayMicroseconds(1000000);// 500ms
}
}
int main(){
    setup();
    while(1){
        pid_tracking();
    }

}