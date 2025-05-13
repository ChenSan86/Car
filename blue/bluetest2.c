#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "wiringx.h" // 假设你已正确安装并包含该库
// scp -O bluetest2 root@192.168.42.1:/root/
int fd = -1;
char ReturnTemp[128];    // 发送缓冲区
char InputString[80];    // 接收缓冲区
int NewLineReceived = 0; // 是否接收到完整命令
int StartBit = 0;        // 是否开始记录
int g_num = 0;           // 当前接收字符位置
int g_packnum = 0;
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

int setupSerial()
{
    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        wiringXGC();
        return -1;
    }
    const char *device = "/dev/ttyS1";
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
void serialEvent()
{

    int UartReturnCount;
    char uartvalue = 0;

    while (1)
    {
        UartReturnCount = wiringXSerialDataAvail(fd);
        if (UartReturnCount == 0)
        {
            break;
        }
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
                InputString[g_num + 1] = '\0'; // 终止字符串
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
void itoa(int i, char *string)
{
    sprintf(string, "%d", i);
    return;
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

// 模拟解析接收指令
void processCommand()
{
    if (NewLineReceived)
    {
        if (StringFind((const char *)InputString, "MODE", 0) > 0)
        {
            int i = StringFind(InputString, "MODE", 0);
            int ii = StringFind(InputString, "#", i);
            if (ii > i && ii - i >= 6) // 至少包含 MODE+2位编号
            {
                char modeStr[4] = {0};
                strncpy(modeStr, &InputString[i + 4], 2); // 从 MODE 后面截取2位数字
                int mode = atoi(modeStr);                 // 转换为整数

                printf("【解析出模式编号】：%d\n", mode);

                switch (mode)
                {
                case 21:
                    printf("→ 启动巡线模式\n");
                    break;
                case 31:
                    printf("→ 启动避障模式\n");
                    break;
                case 41:
                    printf("→ 启动七彩灯模式\n");
                    break;
                case 51:
                    printf("→ 启动寻光模式\n");
                    break;
                case 61:
                    printf("→ 启动跟随模式\n");
                    break;
                default:
                    printf("→ 未知模式：%d\n", mode);
                    break;
                }
            }
            NewLineReceived = 0;
            memset(InputString, 0x00, sizeof(InputString));
            return;
        }
        // 解析上位机发来的舵机云台的控制指令并执行舵机旋转
        // 如:$4WD,PTZ180# 舵机转动到180度
        if (StringFind((const char *)InputString, (const char *)"PTZ", 0) > 0)
        {
            int m_kp, i, ii;
            // 寻找以PTZ开头,#结束中间的字符
            i = StringFind((const char *)InputString, (const char *)"PTZ", 0);
            ii = StringFind((const char *)InputString, (const char *)"#", i);
            if (ii > i)
            {
                char m_skp[5] = {0};
                memcpy(m_skp, InputString + i + 3, ii - i - 3);

                m_kp = atoi(m_skp); // 将找到的字符串变成整型

                printf("舵机转到：%d\n", m_kp); // 转动到指定角度m_kp
                fflush(stdout);
                NewLineReceived = 0;
                memset(InputString, 0x00, sizeof(InputString));
                return;
            }
        }

        // 解析上位机发来的七彩探照灯指令并点亮相应的颜色
        // 如:$4WD,CLR255,CLG0,CLB0# 七彩灯亮红色
        if (StringFind((const char *)InputString, (const char *)"CLR", 0) > 0)
        {
            int m_kp, i, ii, red, green, blue;
            char m_skp[5] = {0};
            i = StringFind((const char *)InputString, (const char *)"CLR", 0);
            ii = StringFind((const char *)InputString, (const char *)",", i);
            if (ii > i)
            {
                memcpy(m_skp, InputString + i + 3, ii - i - 3);
                m_kp = atoi(m_skp);
                red = m_kp;
            }
            i = StringFind((const char *)InputString, (const char *)"CLG", 0);
            ii = StringFind((const char *)InputString, (const char *)",", i);
            if (ii > i)
            {
                memcpy(m_skp, InputString + i + 3, ii - i - 3);
                m_kp = atoi(m_skp);
                green = m_kp;
            }
            i = StringFind((const char *)InputString, (const char *)"CLB", 0);
            ii = StringFind((const char *)InputString, (const char *)"#", i);
            if (ii > i)
            {
                memcpy(m_skp, InputString + i + 3, ii - i - 3);
                m_kp = atoi(m_skp);
                blue = m_kp;
                printf("light:(%d,%d,%d)\n", red, green, blue); // 点亮相应颜色的灯
                fflush(stdout);
                NewLineReceived = 0;
                memset(InputString, 0x00, sizeof(InputString));
                return;
            }
        }

        // 解析上位机发来的通用协议指令,并执行相应的动作
        // 如:$1,0,0,0,0,0,0,0,0,0#    小车前进
        if (StringFind((const char *)InputString, (const char *)"4WD", 0) == -1 &&
            StringFind((const char *)InputString, (const char *)"#", 0) > 0)
        {
            // puts(InputString);
            // 小车原地左旋右旋判断
            if (InputString[3] == '1') // 小车原地左旋
            {
                puts("小车原地左旋");
            }
            else if (InputString[3] == '2') // 小车原地右旋
            {
                puts("小车原地右旋");
            }

            // 小车鸣笛判断
            if (InputString[5] == '1') // 鸣笛
            {
                puts("小车鸣笛");
            }

            // 舵机左旋右旋判断
            if (InputString[9] == '1') // 舵机旋转到180度
            {
                puts("舵机旋转到180度");
            }
            if (InputString[9] == '2') // 舵机旋转到0度
            {
                puts("舵机旋转到0度");
            }

            // 点灯判断
            if (InputString[13] == '1') // 七彩灯亮白色
            {
                puts("七彩灯亮白色");
            }
            if (InputString[13] == '2') // 七彩灯亮红色
            {
                puts("七彩灯亮红色");
            }
            if (InputString[13] == '3') // 七彩灯亮绿灯
            {
                puts("七彩灯亮绿灯");
            }
            if (InputString[13] == '4') // 七彩灯亮蓝灯
            {
                puts("七彩灯亮蓝灯");
            }

            // 灭火判断
            if (InputString[15] == '1') // 灭火
            {
                puts("灭火");
            }

            // 舵机归为判断
            if (InputString[17] == '1') // 舵机旋转到90度
            {
                puts("舵机旋转到90度");
            }
            if (InputString[1] == '1') // 小车前进
            {
                puts("小车前进");
            }
            if (InputString[1] == '2') // 小车后退
            {
                puts("小车后退");
            }
            if (InputString[1] == '3') // 小车左转
            {
                puts("小车左转");
            }
            if (InputString[1] == '4') // 小车右转
            {
                puts("小车右转");
            }

            // 小车的前进,后退,左转,右转,停止动作
            memset(InputString, 0x00, sizeof(InputString)); // 清空串口数据
            NewLineReceived = 0;
        }
    }
}

int main()
{
    if (setupSerial() != 0)
    {
        printf("串口初始化失败\n");
        return 1;
    }
    while (1)
    {
        serialEvent();
        processCommand();
        delayMicroseconds(100);
    }
    return 0;
}