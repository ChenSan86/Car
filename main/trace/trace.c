#include "trace.h"
fd = -1;
void initSernsor()
{
    int fd = wiringXI2CSetup(I2C_DEV, I2C_ADDR);
    if (fd < 0)
    {
        printf("I2C setup failed!\n");
        return -1;
    }
}
void getTrace(int* sernsor_vlaues){
    int data = wiringXI2CReadReg8(fd, 0x30);
    if (data < 0)
    {
        printf("Read failed!\n");
        return;
    }
    for (int i = 0; i < 8; i++)
    {
        sernsor_vlaues[i] = (data >> 7-i) & 1;
    }
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