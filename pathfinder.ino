#include "MsTimer2.h"
#include "stdio.h"
#include "stdlib.h"
#include "U8glib.h"
int cardelay=900;
int LPWM=42;
int RPWM=42;  //初始速度
int addPWMmin=-42;
int addPWMmax=42;
float Kp =70 , Ki = 0.00017, Kd = 87;     //待调参数

int leftCounter=0,  rightCounter=0;
unsigned long time = 0, old_time = 0; // 时间标记
unsigned long time1 = 0; // 时间标记
int servoPin = 11;    //定义舵机接口数字接口11 也就是舵机的橙色信号线。

//Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
//int pos = 100;    // variable to store the servo position 

float lv,rv;//左、右轮速度

#define STOP        0
#define FORWARD     1
#define BACKWARD    2
#define TURNLEFT    3
#define TURNRIGHT   4
#define CHANGESPEED 5

int leftMotor1 = 5;
int leftMotor2 = 6;
int rightMotor1 = 9;
int rightMotor2 = 10;

bool speedLevel=0;

float error = 0, P = 0, I = 0, D = 0, PID_value = 0;
float previous_error = 0, previous_I = 0;
int act=0;
int numsum=0;
int blue=0;
int bluecounter=0;
int timercounter=0;

//U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);        // I2C / TWI 
U8GLIB_SSD1306_128X64 u8g(A5, A4, 4, 6);  // SW SPI Com: SCL = A5, SDA = A4, CS = 4, DC = 6,RES=RESET

void setup() 
{
  MsTimer2::set(cardelay,servotimer);
  
  pinMode(servoPin, OUTPUT); //设定舵机接口为输出接口
  servo(70);  
  delay(500);
  Serial.begin(9600);

  pinMode(leftMotor1, OUTPUT);
  pinMode(leftMotor2, OUTPUT);
  pinMode(rightMotor1, OUTPUT);
  pinMode(rightMotor2, OUTPUT);

  pinMode(12, INPUT);
  pinMode(13, INPUT);
}

void loop()
{
      analogWrite(leftMotor1, 100);
      digitalWrite(leftMotor2, LOW);
      analogWrite(rightMotor1, 100);
      digitalWrite(rightMotor2, LOW);
      delay(50);
while(1)
  {
    char num0,num1,num2,num3,num4,num5,num6,num7;
      num0=digitalRead(14);    
      num1=digitalRead(15);    
      num2=digitalRead(16);
      num3=digitalRead(17);
      num4=digitalRead(3);//原A4
      num5=digitalRead(2);
      num6=digitalRead(13);
      num7=digitalRead(12);      // 用num0-7保存从左到右8个传感器的状态
    numsum=num0+num1+num2+num3+num4+num5+num6+num7;
    if(numsum==0) motorRun(0);
    if(numsum>6) motorRun(0);
    pid(numsum,num0,num1,num2,num3,num4,num5,num6,num7);
    if(numsum>=3) {
       error = 0, P = 0, I = 0, D = 0, PID_value = 0;
       previous_error = 0, previous_I = 0;
       int countLeft=0;
       int countRight=0;
       if(num0==1)
       {countLeft++;}
       if(num1==1)
       {countLeft++;}
       if(num2==1)
       {countLeft++;}
       if(num3==1)
       {countLeft++;}
       if(num4==1)
       {countRight++;}
       if(num5==1)
       {countRight++;}
       if(num6==1)
       {countRight++;}
       if(num7==1)
       {countRight++;}
       
    switch (act)
     {
      case 0:
          {
            if(countRight>countLeft)
              {
                MsTimer2::start();  
                
              motorRun(3);
              delay(300);
              //servo(10);           
              delay(0);  
              act++;
              }
            else if(countLeft>countRight)
              {
                MsTimer2::start();     
                
              motorRun(4);
              delay(400);
              //servo(10);           
              delay(0);   
              act++;act++;
              }

 LPWM=43;
 RPWM=43;  //加速
 addPWMmin=-42;
 addPWMmax=42;

            break;
         }
         
     case 1:
       { timercounter++;   
        MsTimer2::start();  
        motorRun(4);
        delay(600); 
              act=3;
              blue=1;
              break;
        }
     
     case 2:
     {timercounter++;   
      MsTimer2::start();     
      motorRun(3);
          delay(600);
           // servo(10);   
            act=3;
            blue=2;
            break;


 LPWM=50;
 RPWM=50;  //小车初始速度
 addPWMmin=-35;
 addPWMmax=35;
      }
            
     case 3:{motorRun(0);
      while(1)
        {
          u8g.firstPage();  
          do {draw();} while( u8g.nextPage() );
         }
     delay(20000); } //后续加上OLED显示
    }

    if(act>=4)
    {motorRun(0);
    delay(20000);}
      }
   }
}


void motorRun(int cmd)
{
  switch(cmd){
    case FORWARD:
     // Serial.println("FORWARD"); 
     
     // SpeedDetection();
      analogWrite(leftMotor1, LPWM);
      digitalWrite(leftMotor2, LOW);
      analogWrite(rightMotor1, RPWM);
      digitalWrite(rightMotor2, LOW);
      
      break;
     case BACKWARD:
      //Serial.println("BACKWARD"); 
      analogWrite(leftMotor1, LPWM);
      digitalWrite(leftMotor2, LOW);
      analogWrite(rightMotor1, RPWM);
      digitalWrite(rightMotor2, LOW);
      break;
     case TURNLEFT:
     // Serial.println("TURN  LEFT"); 
      analogWrite(leftMotor1,60);
      analogWrite(leftMotor2, 0);
      analogWrite(rightMotor1, 0);
      analogWrite(rightMotor2, 50);
      break;
     case TURNRIGHT:
      //Serial.println("TURN  RIGHT"); 
      analogWrite(leftMotor1, 0);
      analogWrite(leftMotor2, 50);
      analogWrite(rightMotor1, 60);
      analogWrite(rightMotor2, 0);
      break;
     case CHANGESPEED:
     // Serial.println("CHANGE SPEED"); 
      
     default:
      Serial.println("STOP"); 
      digitalWrite(leftMotor1, LOW);
      digitalWrite(leftMotor2, LOW);
      digitalWrite(rightMotor1, LOW);
      digitalWrite(rightMotor2, LOW);
  }
}

void RightCount_CallBack()
{
  rightCounter++;
}
/*
 * *左轮编码器中断服务函数
 */
void LeftCount_CallBack()
{
  leftCounter++;
}

void servo(int angle) { //定义一个脉冲函数
  //发送50个脉冲
  for(int i=0;i<50;i++){
    int pulsewidth = (angle * 11) + 500; //将角度转化为500-2480的脉宽值
    digitalWrite(servoPin, HIGH);   //将舵机接口电平至高
    delayMicroseconds(pulsewidth);  //延时脉宽值的微秒数
    digitalWrite(servoPin, LOW);    //*****将舵机接口电平至低！！！！！
    delayMicroseconds(20000 - pulsewidth);
  }
  delay(100);
}

void pid(int numsum,int num0,int num1,int num2,int num3,int num4,int num5,int num6,int num7)
  {
    //检测到白线返回0，黑线返回1。调试时用白胶带代替黑线。场地调试记得全部改成num*=1.
    if(num0==1)      error=-3;
    else if(num7==1) error=3;
    else if(num1==1) error=-2.7;
    else if(num6==1) error=2.7;
    else if(num2==1) error=-2.3;
    else if(num5==1) error=2.3;
    else if(num3==1) error=0;
    else if(num4==1) error=0;
    
    P = error;
    if(error>=-2.7&&error<=2.7) I = I + error;
    D = error - previous_error;
    PID_value = (Kp * P) + (Ki * I) + (Kd * D);
    PID_value = constrain(PID_value, addPWMmin, addPWMmax);
    previous_error = error;
    
    analogWrite(leftMotor1, LPWM - PID_value);
    digitalWrite(leftMotor2, LOW);
    analogWrite(rightMotor1, RPWM + PID_value);
    digitalWrite(rightMotor2, LOW); //循迹
 }

void servotimer()
{
 if(timercounter==0)
 servo(130);
 else servo(10);
}
