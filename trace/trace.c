#include <stdio.h>
#include <unistd.h>
#include <wiringx.h>
#include <stdint.h>

int main()
{
    const char *i2c_dev = "/dev/i2c-0"; // 与 Python 中 bus = smbus.SMBus(1) 对应
    int i2c_addr = 0x12;                // I2C 设备地址

    // 设置 I2C 设备
    int fd = wiringXI2CSetup(i2c_dev, i2c_addr);
    if (fd < 0)
    {
        printf("I2C setup failed!\n");
        return -1;
    }

    // 写寄存器 0x01 值为 1
    // wiringXI2CWriteReg8(fd, 0x01, 1);
    // sleep(1);

    // 写寄存器 0x01 值为 0
    // wiringXI2CWriteReg8(fd, 0x01, 0);
    // sleep(1);

    // 循环读取寄存器 0x30 的数据
    while (1)
    {
        usleep(500000); // 0.5秒

        int data = wiringXI2CReadReg8(fd, 0x30); // 从寄存器 0x30 读1字节
        if (data < 0)
        {
            printf("Read failed!\n");
            continue;
        }

        // 解析每一位
        printf("x1:%d  x2:%d  x3:%d  x4:%d  x5:%d  x6:%d  x7:%d  x8:%d\n",
               (data >> 7) & 0x01,
               (data >> 6) & 0x01,
               (data >> 5) & 0x01,
               (data >> 4) & 0x01,
               (data >> 3) & 0x01,
               (data >> 2) & 0x01,
               (data >> 1) & 0x01,
               (data >> 0) & 0x01);
    }
    return 0;
}