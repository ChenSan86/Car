#ifndef MOVE_H
#define MOVE_H
extern int speedr, speedl;
int initMove();
void Move(float l, float r);
void brake();
void forward();
void back();
void left();
void right();
void rotationR();
void rotationL();
void whistle();
#endif
