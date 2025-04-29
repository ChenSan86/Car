#include <stdio.h>
#include <wiringx.h>
#include <unistd.h>
#define EN1 14 //forward = 1
#define EN2 15 
#define PWM_1 4 
#define PWM_2 5
#define EN3 16 //forward = 1
#define EN4 17
#define PERIOD 500000 // 500us
#define T 5000
int setup(){
    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        wiringXGC();
        return -1;
    }
    wiringXPWMSetPeriod(PWM_1, PERIOD); // 500us
    wiringXPWMSetPolarity(PWM_1, 0);
    wiringXPWMEnable(PWM_1, 1);
    wiringXPWMSetPeriod(PWM_2, PERIOD); // 500us    
    wiringXPWMSetPolarity(PWM_2,0);      
    wiringXPWMEnable(PWM_2, 1);
    if (wiringXValidGPIO(EN1) != 0)
    {
        printf("Invalid GPIO %d\n", EN1);
    }
    if (wiringXValidGPIO(EN2) != 0)
    {
        printf("Invalid GPIO %d\n", EN2);
    }
    if (wiringXValidGPIO(EN3) != 0)
    {
        printf("Invalid GPIO %d\n", EN3);
    }
    if (wiringXValidGPIO(EN4) != 0)
    {
        printf("Invalid GPIO %d\n", EN4);
    }

    pinMode(EN3, PINMODE_OUTPUT);
    pinMode(EN4, PINMODE_OUTPUT);
    pinMode(EN1, PINMODE_OUTPUT);
    pinMode(EN2, PINMODE_OUTPUT);
    digitalWrite(EN1, LOW);
    digitalWrite(EN2, LOW);
    digitalWrite(EN3, LOW);
    digitalWrite(EN4, LOW);
}
void motor_run(float left,float right){
    if (left > 0)
    {
        digitalWrite(EN1, HIGH);
        digitalWrite(EN2, LOW);
        wiringXPWMSetDuty(PWM_1, (long)left*PERIOD);
    }else if (left < 0)
    {
        digitalWrite(EN1, LOW);
        digitalWrite(EN2, HIGH);
        wiringXPWMSetDuty(PWM_1, -(long)left*PERIOD);
    }else{
        digitalWrite(EN1, LOW);
        digitalWrite(EN2, LOW);
    }
    if (right > 0)
    {
        digitalWrite(EN3, HIGH);
        digitalWrite(EN4, LOW);
        wiringXPWMSetDuty(PWM_2, (long)right*PERIOD);
    }else if (right < 0)
    {
        digitalWrite(EN3, LOW);
        digitalWrite(EN4, HIGH);
        wiringXPWMSetDuty(PWM_2, -right*PERIOD);
    }else{
        digitalWrite(EN3, LOW);
        digitalWrite(EN4, LOW);
    }
}
void stop(){
    motor_run(0,0);
}
void forward(long speed){
    motor_run(speed,speed);
}
void back(long speed){
    motor_run(-speed,-speed);
}
void left(long speed){
    motor_run(-speed,speed);
}
void right(long speed){
    motor_run(speed,-speed);
}
int main(){
    setup();
    motor_run(-30,100);
    delayMicroseconds(3000000);
    stop();
    delayMicroseconds(3000000);
    motor_run(-100,30);
    delayMicroseconds(3000000);
    stop();
    
    return 0;
    
}