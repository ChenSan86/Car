#include "blink.h"
#include "move.h"
#include "trace.h"
#define Kp 1.5
#define Ki 0
#define Kd 2

float er[8] = {3.4, 1.8, 0.8, 0.2, -0.2, -0.8, -1.8, -3.4};
int sensorValues[8]={0};

float calculateError(int *s)
{
    float error = 0;
    float sum = 0;
    for (int i = 0; i < 8; i++)
        if (s[i] == 0)
            error += er[i], sum++;

    if (sum == 0)
        return -100;
    return error / sum;
}
void Auto_tracking_mode() // 自动循迹模式
{
    // PID variables
    float lastError = 0;
    float integral = 0;
    speedl = 95, speedr = 95;
    int presec = 0;
    int fx = 0;
    while (1)
    {
        
        getTrace(sensorValues);
        float error = calculateError(sensorValues);
        if (error == -100)
        {
            while (1)
            {
                getTrace(sensorValues);
                float error = calculateError(sensorValues);
                if (error != -100)
                    break;
                Move(fx, -fx);
                delayMicroseconds(5000);
            }
            continue;
        }
        if (!(sensorValues[0] | sensorValues[1]))
            fx = -2;
        if (!(sensorValues[6] | sensorValues[7]))
            fx = 2;
        float derivative = error - lastError;
        integral += error;
        float speedDifference = Kp * error + Ki * integral + Kd * derivative;
        float leftSpeed = 1.0 - speedDifference;
        float rightSpeed = 1.0 + speedDifference;
        if (leftSpeed > 1)
            leftSpeed = 1;
        if (rightSpeed > 1)
            rightSpeed = 1;
        if (leftSpeed < -1)
            leftSpeed = -1;
        if (rightSpeed < -1)
            rightSpeed = -1;
        Move(leftSpeed, rightSpeed);
        delayMicroseconds(5000);
        lastError = error;
    }
}
int main(){

    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        wiringXGC();
        return -1;
    }
    initMove();
    initBlink();
    initSensor();
    
    Move(0, 0);
    blink(3);

    Auto_tracking_mode();

    return 0;
}