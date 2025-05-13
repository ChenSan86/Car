#ifndef SHIN_H
#define SHIN_H
//循迹
#define I2C_TRACE "/dev/i2c-0"
//蓝牙
#define UART_BLUETOOTH "/dev/ttyS1"
//网络
#define UART_NET "/dev/ttyS3"
//RGB传感器

//舵机
#define PWM_SREVO 50
//BMP280
#define I2C_BMP "/dev/i2c-2"
//L298N
#define PWM_1 7
#define PWM_2 6
#define EN1 15 // forward = 1
#define EN2 14
#define EN3 16 // forward = 1
#define EN4 17
#define PERIOD 500000 // 500us
#define T 5000
//红外传感器
#define RED_L 18
#define RED_R 19
//超声波
#define TRIG 21
#define ECHO 20
//喇叭
#define WHISTLE 8
//blink
#define BLINK 25
//include 
#include <stdio.h>
#include <wiringx.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>
#endif 