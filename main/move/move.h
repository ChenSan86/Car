#ifndef MOVE_H
#define MOVE_H
#include <stdio.h>
#include <wiringx.h>
#include <unistd.h>
#define EN1 14 // forward = 1
#define EN2 15
#define PWM_1 4
#define PWM_2 5
#define EN3 16 // forward = 1
#define EN4 17
#define PERIOD 500000 // 500us
#define T 5000
extern int speedr, speedl;

void initMove();

void motor_run(float l, float r);


#endif