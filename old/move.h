#ifndef MOVE_H
#define MOVE_H
#include <stdio.h>
#include <wiringx.h>
#include <unistd.h>
//===================move=============
#define EN1 14 // forward = 1
#define EN2 15
#define PWM_1 4
#define PWM_2 5
#define EN3 16 // forward = 1
#define EN4 17
#define PERIOD 500000 // 500us
#define T 5000
//===================trace===============
#define I2C_DEV "/dev/i2c-0"
#define I2C_ADDR 0x12
#define REG_DATA 0x30
extern int trace[];
extern int fd;

int setup();

void motor_run(long left, long right);
void stop();
void forward();
void backward();
void left();
void right();
//================trace============
void getTrace();
#endif