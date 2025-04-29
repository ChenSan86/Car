#ifndef TRACE_H
#define TRACE_H
#define I2C_DEV "/dev/i2c-0"
#define I2C_ADDR 0x12
#define REG_DATA 0x30
extern int fd;
void initSensor();
void getTrace(int *sersor_values);
#endif