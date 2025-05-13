#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include "wiringx.h" // 假设你已正确安装并包含该库
int fd = -1;
char ReturnTemp[512] = "$4WD,CSB120,PV8.3,GS214,LF1011,HW11,GM11#";
int counter = 0;

int main()
{
    const char *device = "/dev/ttyS1"; // 串口设备名
    wiringXSerial_t serial_config;
    serial_config.baud = 9600;
    serial_config.databits = 8;
    serial_config.parity = 'n'; // 无校验
    serial_config.stopbits = 1;
    serial_config.flowcontrol = 'n'; // 无流控
    int fd = wiringXSerialOpen(device, serial_config);
    if (fd < 0)
    {
        perror("串口打开失败");
        return 1;
    }
    printf("串口打开成功，开始发送数据...\n");
    wiringXSerialFlush(fd);
    while (1)
    {
        int data = wiringXSerialDataAvail(fd);
        if (data > 0)
        {
            int a = wiringXSerialGetChar(fd);
            printf("%c", a);
            fflush(stdout);
        }
    }
    wiringXSerialClose(fd);
    return 0;
}