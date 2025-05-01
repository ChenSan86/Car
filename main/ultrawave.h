#ifndef ULTRAVEWAVE_H
#define ULTRAVEWAVE_H
#define TRIG 6 // 触发引脚（输出）
#define ECHO 7 // 回响引脚（输入）
long getMicros();
void initUltr();
double getDistance();
#endif
