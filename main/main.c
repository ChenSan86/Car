#include "move.h"
#include <wiringx.h>
#include "trace.h"
#include <stdio.h>
#include "blink.h"

#define Kp 1.5
#define Ki 0
#define Kd 1

float er[8] = {3.5, 1.2, 0.7, 0.4, -0.4, -0.7, -1.2, -3.5};

float calculateError(int *s)
{
    float error = 0;
    float sum = 0;
    for (int i = 0; i < 8; i++)
        if (s[i] == 0)
            error += er[i], sum++;

    if (sum == 0)
        return -100;
    for (int i = 0; i < 8; ++i)
        printf("%d ", s[i]);
    printf("\n");
    return error / sum;
}
void Auto_tracking_mode() // 自动循迹模式
{
    // PID variables
    float lastError = 0;
    float integral = 0;
    speedl = 100, speedr = 100;
    int presec = 0;
    int fx = 0;
    while (1)
    {
        int sensorValues[8];
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
                printf("fx:%f", fx);
                delayMicroseconds(5000);
            }
            continue;
        }
        if (!(sensorValues[0] | sensorValues[1]))
            fx = -1;
        if (!(sensorValues[6] | sensorValues[7]))
            fx = 1;
        float derivative = error - lastError;
        integral += error;
        float speedDifference = Kp * error + Ki * integral + Kd * derivative;
        printf("error = %f, speedDifference = %f\n", error, speedDifference);
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
        printf("leftSpeed = %f, rightSpeed = %f\n", leftSpeed, rightSpeed);
        Move(leftSpeed, rightSpeed);
        delayMicroseconds(5000); 
        lastError = error;
        //  delay(5);
    }
    
}
int main(){
    initMove();
    initSensor();
    blink();
    Auto_tracking_mode();
    return 0;
}
