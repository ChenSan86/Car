#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "move.h"
#include "trace.h"
#include "wiringx.h"
#include "blink.h"
#define run_car '1'   // 按键前
#define back_car '2'  // 按键后
#define left_car '3'  // 按键左
#define right_car '4' // 按键右
#define stop_car '0'  // 按键停
void sendData(char *time);
enum
{
    enSTOP = 0,
    enRUN,
    enBACK,
    enLEFT,
    enRIGHT,
    enTLEFT,
    enTRIGHT
} enCarState;
int g_CarState = enSTOP;

// 全局变量和线程控制
static time_t start_time;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int tracking_active = 0; // 控制跟踪线程是否运行
int should_exit = 0;     // 控制线程是否退出

// 串口相关变量
int fd = -1;
char ReturnTemp[128];
char InputString[80];
int NewLineReceived = 0;
int StartBit = 0;
int g_num = 0;
int g_packnum = 0;

// PID控制参数
#define Kp 1.5
#define Ki 0
#define Kd 2
float er[8] = {3.4, 1.8, 0.8, 0.2, -0.2, -0.8, -1.8, -3.4};
//{3.4, 1.2, 0.7, 0.4, -0.4, -0.7, -1.2, -3.4};
// 3.4 1.2
int setupSerial()
{
    printf("setupSerial\n");
    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        wiringXGC();
        return -1;
    }
    const char *device = "/dev/ttyS4";
    wiringXSerial_t serial_config;
    serial_config.baud = 9600;
    serial_config.databits = 8;
    serial_config.parity = 'n'; // 无校验
    serial_config.stopbits = 1;
    serial_config.flowcontrol = 'n'; // 无流控
    fd = wiringXSerialOpen(device, serial_config);
    if (fd < 0)
    {
        perror("串口打开失败");
        return 1;
    }
    printf("串口打开成功，开始接收数据...\n");
    wiringXSerialFlush(fd);
    return 0;
}

float calculateError(int *s)
{
    float error = 0;
    float sum = 0;
    for (int i = 0; i < 8; i++)
        if (s[i] == 0)
            error += er[i], sum++;

    if (sum == 0)
        return -100;;
    return error / sum;
}
// 计时器函数
void start_timer()
{
    printf("start timer\n");
    time(&start_time);
}

char *stop_timer()
{
    time_t end_time;
    time(&end_time);

    double elapsed_seconds = difftime(end_time, start_time);
    int minutes = (int)(elapsed_seconds / 60);
    int seconds = (int)(elapsed_seconds) % 60;

    char *result = (char *)malloc(5 * sizeof(char));
    if (result == NULL)
    {
        return NULL;
    }
    snprintf(result, 5, "%02d%02d", minutes, seconds);
    return result;
}

// 自动跟踪线程函数
void *auto_tracking_thread(void *arg)
{
    float lastError = 0;
    float integral = 0;
    printf("auto tracking thread start\n");
    speedl = 100, speedr = 100;
    int presec = 0;
    int fx = 0;

    while (!should_exit)
    {
        pthread_mutex_lock(&lock);
        while (!tracking_active && !should_exit)
        {
            pthread_cond_wait(&cond, &lock);
        }
        pthread_mutex_unlock(&lock);

        if (should_exit)
            break;

        // 执行跟踪任务
        int sensorValues[8];
        getTrace(sensorValues);
        float error = calculateError(sensorValues);
        if (error == -100)
        {
            while (1)
            {
                getTrace(sensorValues);
                error = calculateError(sensorValues);
                if (error != -100)
                    break;
                Move(fx, -fx);
                //printf("fx:%f", fx);
                delayMicroseconds(5000);

                pthread_mutex_lock(&lock);
                if (!tracking_active || should_exit)
                    break;
                pthread_mutex_unlock(&lock);
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

        // 限制速度范围
        leftSpeed = (leftSpeed > 1) ? 1 : ((leftSpeed < -1) ? -1 : leftSpeed);
        rightSpeed = (rightSpeed > 1) ? 1 : ((rightSpeed < -1) ? -1 : rightSpeed);

        Move(leftSpeed, rightSpeed);
        delayMicroseconds(5000);
        
        lastError = error;
    }

    return NULL;
}

// 串口处理函数
void serialEvent()
{
    int UartReturnCount;
    char uartvalue = 0;

    while (1)
    {
        UartReturnCount = wiringXSerialDataAvail(fd);
        if (UartReturnCount == 0)
            break;

        while (UartReturnCount--)
        {
            uartvalue = wiringXSerialGetChar(fd);
            if (uartvalue == '$')
            {
                StartBit = 1;
                g_num = 0;
            }
            if (StartBit == 1)
            {
                InputString[g_num] = uartvalue;
            }
            if (StartBit == 1 && uartvalue == '#')
            {
                NewLineReceived = 1;
                StartBit = 0;
                g_packnum = g_num;
                InputString[g_num + 1] = '\0';
            }
            g_num++;
            if (g_num >= 80)
            {
                g_num = 0;
                StartBit = 0;
                NewLineReceived = 0;
            }
        }
    }
}
int StringFind(const char *pSrc, const char *pDst, int v_iStartPos)
{
    int i, j;
    for (i = v_iStartPos; pSrc[i] != '\0'; i++)
    {
        if (pSrc[i] != pDst[0])
            continue;
        j = 0;
        while (pDst[j] != '\0' && pSrc[i + j] != '\0')
        {
            j++;
            if (pDst[j] != pSrc[i + j])
                break;
        }
        if (pDst[j] == '\0')
            return i;
    }
    return -1;
}

// 命令处理函数
void processCommand()
{
    if (NewLineReceived)
    {
        if (StringFind((const char *)InputString, (const char *)"4WD", 0) == -1 &&
            StringFind((const char *)InputString, (const char *)"#", 0) > 0)
        {

            if (InputString[3] == '1')
            { // 开始跟踪
                pthread_mutex_lock(&lock);
                tracking_active = 1;
                start_timer();
                pthread_cond_signal(&cond);
                pthread_mutex_unlock(&lock);
                printf("跟踪模式启动\n");
            }
            else if (InputString[3] == '2')
            { // 停止跟踪
                pthread_mutex_lock(&lock);
                tracking_active = 0;
                char *elapsed_time = stop_timer();
                printf("elapsed_time:%s\n", elapsed_time);
                sendData(elapsed_time);
                free(elapsed_time);
                pthread_mutex_unlock(&lock);
                printf("跟踪模式停止\n");
            }
            if (g_CarState != enTLEFT && g_CarState != enTRIGHT)
            {
                switch (InputString[1])
                {
                case run_car:
                    g_CarState = enRUN;
                    break;
                case back_car:
                    g_CarState = enBACK;
                    break;
                case left_car:
                    g_CarState = enLEFT;
                    break;
                case right_car:
                    g_CarState = enRIGHT;
                    break;
                case stop_car:
                    g_CarState = enSTOP;
                    break;
                default:
                    g_CarState = enSTOP;
                    break;
                }
            }

            memset(InputString, 0x00, sizeof(InputString)); // 清空串口数据
            NewLineReceived = 0;

            // 根据小车状态做相应的动作
            switch (g_CarState)
            {
            case enSTOP:
                brake();
                break;
            case enRUN:
                forward();
                break;
            case enLEFT:
                left();
                break;
            case enRIGHT:
                right();
                break;
            case enBACK:
                back();
                break;
            default:
                brake();
                break;
            }

            memset(InputString, 0x00, sizeof(InputString));
            NewLineReceived = 0;
        }
    }
}

// 数据发送函数
void sendData(char *time)
{
    wiringXSerialFlush(fd);
    wiringXSerialPrintf(fd, "$4WD,CSB%s,PV8.3,GS214,LF1011,HW11,GM11#\n", time);
    delayMicroseconds(10000);
}

// 主函数
int main()
{
    pthread_t tracking_thread;

    // 初始化硬件
    initMove();
    initSensor();
    initBlink();
    blink(1);
    if (setupSerial() != 0)
    {
        fprintf(stderr, "初始化失败\n");
        return 1;
    }

    // 创建跟踪线程
    if (pthread_create(&tracking_thread, NULL, auto_tracking_thread, NULL) != 0)
    {
        perror("无法创建跟踪线程");
        return 1;
    }
    sendData("0000");

    // 主循环处理串口数据
    while (!should_exit)
    {
        serialEvent();
        processCommand();
    }

    // 清理资源
    pthread_mutex_lock(&lock);
    should_exit = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock);

    pthread_join(tracking_thread, NULL);
    wiringXSerialClose(fd);
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
    Move(0, 0);
    return 0;
}