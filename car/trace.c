#include "shin.h"
#include "trace.h"
int fdsensor = -1;
static char data[8];
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
    int a = wiringXI2CReadReg8(fdsensor, 0x30); // 从寄存器 0x30 读1字节
    if (a < 0)
    {
        printf("Read failed!\n");
        return;
    }
    s[0] = (a >> 7) & 0x01;
    s[1] = (a >> 6) & 0x01;
    s[2] = (a >> 5) & 0x01;
    s[3] = (a >> 4) & 0x01;
    s[4] = (a >> 3) & 0x01;
    s[5] = (a >> 2) & 0x01;
    s[6] = (a >> 1) & 0x01;
    s[7] = (a >> 0) & 0x01;
}
char *getTraceData(){
    int s[8];
    getTrace(s);
    sprintf(data, "TRACK:%d%d%d%d%d%d%d%d", s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7]);
    return data;
}