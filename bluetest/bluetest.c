#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "wiringx.h" // 假设你已正确安装并包含该库

int main()
{
    const char *device = "/dev/ttyS4"; // 串口设备名
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
    printf("串口打开成功，开始接收数据...\n");
    wiringXSerialFlush(fd);
    while (1)
    {
        if (wiringXSerialDataAvail(fd) > 0)
        {
            int c = wiringXSerialGetChar(fd);
            if (c != -1)
            {
                putchar(c); // 打印收到的字符
                fflush(stdout);
            }
        }
        else
        {
            usleep(1000); // 无数据时稍作等待，避免CPU占用过高
        }
    }
    wiringXSerialClose(fd);
    return 0;
}