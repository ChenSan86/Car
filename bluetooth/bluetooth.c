#include <stdio.h>
#include <wiringx.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
int fd;
/*串口长度变量*/
int g_num = 0;
int g_packnum = 0;
/*串口数据设置*/
char InputString[512] = {0}; // 用来储存接收到的内容
int NewLineReceived = 0;     // 前一次数据结束标志
int StartBit = 0;            // 协议开始标志
char ReturnTemp[512] = {0};  // 存储返回值

void serialEvent()
{
    int UartReturnCount;
    char uartvalue = 0;
    while (1)
    {
        UartReturnCount = wiringXSerialDataAvail(fd);
        if (UartReturnCount == 0)
        {
            puts("UartReturnCount out");
            delayMicroseconds(1000000); // 1s
            break;
        }
        else if (UartReturnCount > 0)
        {
            int counter = UartReturnCount;
            while (counter--)
            {
                puts("UartReturnCount in");
                uartvalue = (char)wiringXSerialGetChar(fd);
                if(counter == UartReturnCount){
                    puts("dirty data");
                    g_num = 0;
                    StartBit = 0;
                    NewLineReceived = 0;
                }
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
                    printf("inputstring:%s\n", InputString);
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
    delayMicroseconds(100);
}
int setupSerial(){
    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        wiringXGC();
        puts("wiringXSetup failed");
        return -1;
    }
    struct wiringXSerial_t serial;
    serial.baud = 9600;  // 蓝牙模块常用波特率
    serial.databits = 8; // 数据位8位
    serial.parity = 'n'; // 无校验
    serial.stopbits = 1; // 1位停止位
    serial.flowcontrol = 'n';
    printf("Waiting for data...\n");
    fd = wiringXSerialOpen("/dev/ttyS0", serial);
    if (fd < 0)
    {
        puts("open serial failed");
        return -1;
    }
    
    puts("fd open successfully");
    return 1;
}

int main()
{
    int setup = setupSerial();
    if (setup == -1)
    {
        printf("setup failed\n");
        return -1;
    }
    while (setup == 1)
    {
        serialEvent();
        
        memset(InputString, 0x00, sizeof(InputString)); // 清空串口数据
        NewLineReceived = 0;
    }
    wiringXSerialClose(fd);
    return 0;
}