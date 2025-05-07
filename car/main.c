#include "global.h"
//小车运动状态
#define run_car '1'   // 按键前
#define back_car '2'  // 按键后
#define left_car '3'  // 按键左
#define right_car '4' // 按键右
#define stop_car '0'  // 按键停
int fd_bluetooth;
int init_blue();
int initCar(){
    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        wiringXGC();
        return 1;
    }
    init_blue();
    initServo();
    init_bmp280();
    init_tcs34725();
    initDistance();
    initTCP();
    initSensor();
    return 0;
}
int init_blue(){
    wiringXSerial_t serial_config;
    serial_config.baud = 9600;
    serial_config.databits = 8;
    serial_config.parity = 'n'; // 无校验
    serial_config.stopbits = 1;
    serial_config.flowcontrol = 'n'; // 无流控
    fd_bluetooth = wiringXSerialOpen(UART_BLUETOOTH, serial_config);
    if (fd_bluetooth == -1)
    {
        printf("Failed to open serial port\n");
        return -1;
    }
    return 0;
};
enum
{
    enSTOP = 0,
    enRUN,
    enBACK,
    enLEFT,
    enRIGHT,
    enTLEFT,
    enTRIGHT
} enCarState;
//bluetooth control
/*串口数据设置*/
char InputString[512] = {0}; // 用来储存接收到的内容
int NewLineReceived = 0;     // 前一次数据结束标志
int StartBit = 0;            // 协议开始标志
int g_CarState = enSTOP;     // 1前2后3左4右0停止
char ReturnTemp[512] = {0};  // 存储返回值
/*串口长度变量*/
int g_num = 0;
int g_packnum = 0;
int StringFind(const char *pSrc, const char *pDst, int v_iStartPos)
{
    int i, j;
    for (i = v_iStartPos; pSrc[i] != '\0'; i++)
    {
        if (pSrc[i] != pDst[0])
            continue;
        j = 0;
        while (pDst[j] != '\0' && pSrc[i + j] != '\0')
        {
            j++;
            if (pDst[j] != pSrc[i + j])
                break;
        }
        if (pDst[j] == '\0')
            return i;
    }
    return -1;
}
void serial_data_parse()
{
    // 解析上位机发来的舵机云台的控制指令并执行舵机旋转
    // 如:$4WD,PTZ180# 舵机转动到180度
    if (StringFind((const char *)InputString, (const char *)"PTZ", 0) > 0)
    {
        int m_kp, i, ii;
        // 寻找以PTZ开头,#结束中间的字符
        i = StringFind((const char *)InputString, (const char *)"PTZ", 0);
        ii = StringFind((const char *)InputString, (const char *)"#", i);
        if (ii > i)
        {
            char m_skp[5] = {0};
            memcpy(m_skp, InputString + i + 3, ii - i - 3);
            m_kp = atoi(m_skp); // 将找到的字符串变成整型
            turn(m_kp); // 转动到指定角度m_kp
            NewLineReceived = 0;
            memset(InputString, 0x00, sizeof(InputString));
            return;
        }
    }

    // 解析上位机发来的七彩探照灯指令并点亮相应的颜色
    // 如:$4WD,CLR255,CLG0,CLB0# 七彩灯亮红色
    // if (StringFind((const char *)InputString, (const char *)"CLR", 0) > 0)
    // {
    //     int m_kp, i, ii, red, green, blue;
    //     char m_skp[5] = {0};
    //     i = StringFind((const char *)InputString, (const char *)"CLR", 0);
    //     ii = StringFind((const char *)InputString, (const char *)",", i);
    //     if (ii > i)
    //     {
    //         memcpy(m_skp, InputString + i + 3, ii - i - 3);
    //         m_kp = atoi(m_skp);
    //         red = m_kp;
    //     }
    //     i = StringFind((const char *)InputString, (const char *)"CLG", 0);
    //     ii = StringFind((const char *)InputString, (const char *)",", i);
    //     if (ii > i)
    //     {
    //         memcpy(m_skp, InputString + i + 3, ii - i - 3);
    //         m_kp = atoi(m_skp);
    //         green = m_kp;
    //     }
    //     i = StringFind((const char *)InputString, (const char *)"CLB", 0);
    //     ii = StringFind((const char *)InputString, (const char *)"#", i);
    //     if (ii > i)
    //     {
    //         memcpy(m_skp, InputString + i + 3, ii - i - 3);
    //         m_kp = atoi(m_skp);
    //         blue = m_kp;
    //         color_led_pwm(red, green, blue); // 点亮相应颜色的灯
    //         NewLineReceived = 0;
    //         memset(InputString, 0x00, sizeof(InputString));
    //         return;
    //     }
    // }

    // 解析上位机发来的通用协议指令,并执行相应的动作
    // 如:$1,0,0,0,0,0,0,0,0,0#    小车前进
    if (StringFind((const char *)InputString, (const char *)"4WD", 0) == -1 &&
        StringFind((const char *)InputString, (const char *)"#", 0) > 0)
    {
        if (InputString[3] == '1') // 小车原地左旋
        {
            g_CarState = enTLEFT;
        }
        else if (InputString[3] == '2') // 小车原地右旋
        {
            g_CarState = enTRIGHT;
        }
        else
        {
            g_CarState = enSTOP;
        }

        // 小车鸣笛判断
        if (InputString[5] == '1') // 鸣笛
        {
            whistle();
        }


        // 舵机左旋右旋判断
        if (InputString[9] == '1') // 舵机旋转到180度
        {
            turnRight();
        }
        if (InputString[9] == '2') // 舵机旋转到0度
        {
            turnLeft();
        }
        if (InputString[13] == '1') // 七彩灯亮白色
        {
            color_led_pwm(255, 255, 255);
        }
        if (InputString[13] == '2') // 七彩灯亮红色
        {
            color_led_pwm(255, 0, 0);
        }
        if (InputString[13] == '3') // 七彩灯亮绿灯
        {
            color_led_pwm(0, 255, 0);
        }
        if (InputString[13] == '4') // 七彩灯亮蓝灯
        {
            color_led_pwm(0, 0, 255);
        }

        // 灭火判断
        if (InputString[15] == '1') // 灭火
        {
            //TODO: 灭火
        }

        // 舵机归为判断
        if (InputString[17] == '1') // 舵机旋转到90度
        {
            center();
        }

        // 小车的前进,后退,左转,右转,停止动作
        if (g_CarState != enTLEFT && g_CarState != enTRIGHT)
        {
            switch (InputString[1])
            {
            case run_car:
                g_CarState = enRUN;
                break;
            case back_car:
                g_CarState = enBACK;
                break;
            case left_car:
                g_CarState = enLEFT;
                break;
            case right_car:
                g_CarState = enRIGHT;
                break;
            case stop_car:
                g_CarState = enSTOP;
                break;
            default:
                g_CarState = enSTOP;
                break;
            }
        }

        memset(InputString, 0x00, sizeof(InputString)); // 清空串口数据
        NewLineReceived = 0;

        // 根据小车状态做相应的动作
        switch (g_CarState)
        {
        case enSTOP:
            brake();
            break;
        case enRUN:
            run();
            break;
        case enLEFT:
            left();
            break;
        case enRIGHT:
            right();
            break;
        case enBACK:
            back();
            break;
        case enTLEFT:
            rotationL;
            break;
        case enTRIGHT:
            rotationR;
            break;
        default:
            brake();
            break;
        }
    }
}
void itoa(int i, char *string)
{
    sprintf(string, "%d", i);
    return;
}

void serial_data_postback()
{
    //$4WD,CSB120,PV8.3,GS214,LF1011,HW11,GM11#
    char *p = ReturnTemp;
    char str[25];
    float distance;
    memset(ReturnTemp, 0, sizeof(ReturnTemp));
    distance = getDistance();
    if ((int)distance == -1)
    {
        distance = 0;
    }
    strcat(p, "$4WD,CSB");
    itoa((int)distance, str);
    strcat(p, str);
    strcat(p, get_bmp280_data());
    strcat(p, ",LF");
    strcat(p, getTraceData());
    // 红外避障
    strcat(p, ",HW");
    strcat(p, getRL());
    // 光传感器
    strcat(p, ",RGB");
    strcat(p, get_tcs34725_data());
    strcat(p, ",PEOPLE");
    //TODO: 人检测

    strcat(p, "#");
    // 将ReturnTemp所指的内容写入到串口设备中

    printf("ReturnTemp:%s\n", p);
    sendTCP(p);
    return;
}
void serialEvent()
{
    int UartReturnCount;
    char uartvalue = 0;
    while (1)
    {
        UartReturnCount = serialDataAvail();
        if (UartReturnCount == 0)
        {
            break;
        }
        else if (UartReturnCount > 0)
        {
            while (UartReturnCount--)
            {
                uartvalue = (char)serialGetchar(fd_bluetooth);
                if (uartvalue == '$')
                {
                    StartBit = 1;
                    g_num = 0;
                }
                if (StartBit == 1)
                {
                    InputString[g_num] = uartvalue;
                }
                if (StartBit == 1 && uartvalue == '#')
                {
                    NewLineReceived = 1;
                    StartBit = 0;
                    g_packnum = g_num;
                    //  printf("inputstring:%s\n", InputString);
                }
                g_num++;

                if (g_num >= 80)
                {
                    g_num = 0;
                    StartBit = 0;
                    NewLineReceived = 0;
                }
            }
        }
    }
}
//TEST
void bmp280_test(){
    int counter = 10;
    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        wiringXGC();
        return 1;
    }
    init_bmp280();
    printf("bmp280 test\n");
    while(counter --){
        printf("bmp280 data:%s\n", get_bmp280_data());
        delayMicroseconds(1000000);
    }
}
void tcs34725_test(){
    int counter = 10;
    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        wiringXGC();
        return 1;
    }
    init_tcs34725();
    printf("tcs34725 test\n");
    while(counter --){
        printf("tcs34725 data:%s\n", get_tcs34725_data());
        delayMicroseconds(1000000);
    }
}
void distance_test(){
    int counter = 10;
    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        wiringXGC();
        return 1;
    }
    initDistance();
    printf("distance test\n");
    while(counter --){
        printf("distance data:%d\n", getDistance());
        delayMicroseconds(1000000);
    }
}
void trace_test(){
    int counter = 10;
    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        wiringXGC();
        return 1;
    }
    initSensor();
    printf("trace test\n");
    while(counter --){
        printf("trace data:%s\n", getTraceData());
        delayMicroseconds(1000000);
    }
}
void infrared_test(){
    int counter = 10;
    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        wiringXGC();
        return 1;
    }
    initSensor();
    printf("infrared test\n");
    while(counter --){
        printf("infrared data:%s\n", getRL());
        delayMicroseconds(1000000);
    }
}
void servo_test(){
    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        wiringXGC();
        return 1;
    }
    initServo();
    printf("servo test\n");
    center();
    delayMicroseconds(1000000);
    turnRight();
    delayMicroseconds(1000000);
    turnLeft();
    delayMicroseconds(1000000);
    center();
    delayMicroseconds(1000000);
}
//TEST
int main(){
}
