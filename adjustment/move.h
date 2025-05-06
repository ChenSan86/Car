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
#include "wiringx.h"
extern int speedr, speedl;

void initMove();
void Move(float l, float r);

void brake();
void forward();
void back();
void left();
void right();
void rotationR();
void rotationL();
#endif