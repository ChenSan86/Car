#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "wiringx.h" // 假设你已正确安装并包含该库
char ReturnTemp[128];
int main()
{
    int counter = 0;
    int distance, voltage, grayscale, track_val, avoid_val, ldr_val = 0;
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
        distance= voltage=grayscale=track_val=avoid_val=ldr_val = counter;
        snprintf(ReturnTemp, 128,
                 "$4WD,CSB%d,PV%d,GS%d,LF%d,HW%d,GM%d#",
                 (int)distance, voltage, grayscale, track_val, avoid_val, ldr_val);
        printf("%s", ReturnTemp);
        wiringXSerialPuts(fd, ReturnTemp);
        wiringXSerialFlush(fd);
        usleep(1000000); // 1s
        counter++;
    }
    wiringXSerialClose(fd);
    return 0;
}
