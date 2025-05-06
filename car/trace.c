#include "shin.h"
#include "trace.h"
int fdsensor = -1;
void initSensor()
{

    int i2c_addr = 0x12;                // I2C 设备地址

    // 设置 I2C 设备
    fdsensor = wiringXI2CSetup(I2C_TRACE, i2c_addr);
    if (fdsensor < 0)
    {
        printf("I2C setup failed!\n");
    }
}
void getTrace(int *s)
{
    int data = wiringXI2CReadReg8(fdsensor, 0x30); // 从寄存器 0x30 读1字节
    if (data < 0)
    {
        printf("Read failed!\n");
        return;
    }
    s[0] = (data >> 7) & 0x01;
    s[1] = (data >> 6) & 0x01;
    s[2] = (data >> 5) & 0x01;
    s[3] = (data >> 4) & 0x01;
    s[4] = (data >> 3) & 0x01;
    s[5] = (data >> 2) & 0x01;
    s[6] = (data >> 1) & 0x01;
    s[7] = (data >> 0) & 0x01;
}