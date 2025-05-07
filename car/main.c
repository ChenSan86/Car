#include "global.h"
//小车运动状态
#define run_car '1'   // 按键前
#define back_car '2'  // 按键后
#define left_car '3'  // 按键左
#define right_car '4' // 按键右
#define stop_car '0'  // 按键停

void serialEvent();
void serial_data_parse_control();
void serial_data_parse_face();
void serial_data_parse_avoid();
void serial_data_parse_follow();
void serial_data_parse_greenred();
void control();
void avoid();
void follow();
void face();
void greenred();


int if_control = 0;
int if_avoid = 0;
int if_follow = 0;
int if_face = 0;
int if_greenred = 0;

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
void itoa(int i, char *string)
{
    sprintf(string, "%d", i);
    return;
}
//=============================control====================================
void control(){
    while(if_control){
        serialEvent(fd_bluetooth);
        serial_data_parse_control();
    }
}

void serial_data_parse_control(){
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
            turn(m_kp);         // 转动到指定角度m_kp
            NewLineReceived = 0;
            memset(InputString, 0x00, sizeof(InputString));
            return;
        }
    }
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
        

        // 灭火判断
        if (InputString[15] == '1') // 灭火
        {
            // TODO: 灭火
            if_control = 0;
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
            forward();
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
            rotationL();
            break;
        case enTRIGHT:
            rotationR();
            break;
        default:
            brake();
            break;
        }
    }
}
// //=============================avoid====================================
// void avoid(){
//     while(if_avoid){
//         serialEvent(fd_bluetooth);
//         serial_data_parse_avoid();
//     }
// }
// void serial_data_parse_avoid(){
    
// }
// //=============================follow====================================
// void serial_data_parse_follow(){
//     if (StringFind((const char *)InputString, (const char *)"4WD", 0) == -1 &&
//         StringFind((const char *)InputString, (const char *)"#", 0) > 0)
//     {
//         // miehuo
//         if (InputString[15] == '1') // 灭火
//         {
//             // TODO: 灭火
//             exit(0);
//         }
//         NewLineReceived = 0;
//         memset(InputString, 0x00, sizeof(InputString));
//         return;
//     }
// }
// void follow(){
    
// }
// // 跟踪控制参数
// const float targetDistance = 30.0; // 目标跟随距离(cm)
// const float baseSpeed = 0.5;       // 基础跟随速度(0-1)
// const float turnGain = 0.3;        // 转向灵敏度(0-1)
// const float maxDistance = 100.0;   // 最大有效距离(cm)
// const float minDistance = 15.0;    // 最小安全距离(cm)

// // 状态变量
// float lastLeftIR = 0;
// float lastRightIR = 0;

// // 智能跟踪主函数
// void smartTracking()
// {
//     // 1. 获取当前传感器数据
//     float distance = getDistance(); // 超声波距离
//     int leftIR = getL();            // 左侧红外(0/1)
//     int rightIR = getR();           // 右侧红外(0/1)

//     // 2. 更新红外检测状态(带滤波)
//     lastLeftIR = updateIRSmoothing(lastLeftIR, leftIR);
//     lastRightIR = updateIRSmoothing(lastRightIR, rightIR);

//     // 3. 安全边界检查
//     if (distance < minDistance)
//     {
//         move(0, 0); // 紧急停止
//         return;
//     }

//     if (distance > maxDistance)
//     {
//         move(0, 0); // 超出范围停止
//         return;
//     }

//     // 4. 计算控制输出
//     float leftSpeed, rightSpeed;
//     calculateMovement(distance, lastLeftIR, lastRightIR, leftSpeed, rightSpeed);

//     // 5. 执行移动
//     move(leftSpeed, rightSpeed);

//     // 6. 调试输出(可选)
//     debugOutput(distance, lastLeftIR, lastRightIR, leftSpeed, rightSpeed);
// }

// // 红外传感器状态更新(带滤波)
// float updateIRSmoothing(float lastValue, int currentReading)
// {
//     // 当前检测到则设为1，否则缓慢衰减
//     return currentReading ? 1.0 : max(0.0, lastValue - 0.1);
// }

// // 计算运动控制量
// void calculateMovement(float distance, float leftIR, float rightIR,
//                        float leftSpeed, float rightSpeed)
// {
//     // 计算距离误差 (归一化到[-1,1])
//     float distanceError = (distance - targetDistance) / targetDistance;

//     // 计算方向误差 (基于两侧红外检测)
//     float directionError = rightIR - leftIR; // 正值表示目标偏右

//     // 计算基础速度 (距离越接近目标速度越小)
//     float speed = constrain(baseSpeed * (1.0 - abs(distanceError)), 0.2, baseSpeed);

//     // 计算转向调整
//     float turn = turnGain * directionError;

//     // 基础运动控制
//     leftSpeed = constrain(speed + turn, -1.0, 1.0);
//     rightSpeed = constrain(speed - turn, -1.0, 1.0);

//     // 增强转向逻辑 - 当目标明显偏向一侧时
//     if (leftIR > 0.8 && rightIR < 0.2)
//     {
//         leftSpeed = -0.3; // 向左急转
//         rightSpeed = 0.5;
//     }
//     else if (rightIR > 0.8 && leftIR < 0.2)
//     {
//         leftSpeed = 0.5; // 向右急转
//         rightSpeed = -0.3;
//     }
// }

// // 调试信息输出(可选)
// void debugOutput(float dist, float lIR, float rIR, float lSpeed, float rSpeed)
// {

// }


// //=============================face====================================
// void serial_data_parse_face(){
    
// }
// void faceDetect(){
    
// }
// //=============================greenred====================================
// void serial_data_parse_greenred(){
//     if (StringFind((const char *)InputString, (const char *)"4WD", 0) == -1 &&
//         StringFind((const char *)InputString, (const char *)"#", 0) > 0)
//     {
//         // miehuo
//         if (InputString[15] == '1') // 灭火
//         {
//             // TODO: 灭火
//             exit(0);
//         }
//         NewLineReceived = 0;
//         memset(InputString, 0x00, sizeof(InputString));
//         return;
//     }
// }
// void greenredDetect(){
    
    
// }
//=============================main====================================

void serial_data_parse()
{
    if (StringFind((const char *)InputString, "MODE", 0) > 0)
    {
        int i = StringFind(InputString, "MODE", 0);
        int ii = StringFind(InputString, "#", i);
        if (ii > i && ii - i >= 6) // 至少包含 MODE+2位编号
        {
            char modeStr[4] = {0};
            strncpy(modeStr, &InputString[i + 4], 2); // 从 MODE 后面截取2位数字
            int mode = atoi(modeStr);                 // 转换为整数

            printf("【解析出模式编号】：%d\n", mode);

            switch (mode)
            {
            case 21:
                printf("→ 启动巡线模式\n");
                break;
            case 31:
                printf("→ 启动避障模式\n");
                break;
            case 41:
                printf("→ 启动七彩灯模式\n");
                break;
            case 51:
                printf("→ 启动寻光模式\n");
                break;
            case 61:
                printf("→ 启动跟随模式\n");
                break;
            default:
                printf("→ 未知模式：%d\n", mode);
                break;
            }
        }
        NewLineReceived = 0;
        memset(InputString, 0x00, sizeof(InputString));
        return;
    }
    if (StringFind((const char *)InputString, (const char *)"4WD", 0) == -1 &&
        StringFind((const char *)InputString, (const char *)"#", 0) > 0)
    {
        //miehuo
        if (InputString[15] == '1') // 灭火
        {
            // TODO: 灭火
            exit(0);
        }
        NewLineReceived = 0;
        memset(InputString, 0x00, sizeof(InputString));
        return;
    }
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
        UartReturnCount = wiringXSerialDataAvail(fd_bluetooth);
        if (UartReturnCount == 0)
        {
            break;
        }
        else if (UartReturnCount > 0)
        {
            while (UartReturnCount--)
            {
                uartvalue = (char)wiringXSerialGetChar(fd_bluetooth);
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



//TEST
int main(){
    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        wiringXGC();
        return -1;
    }
    distance_test();
    return 0;
}
