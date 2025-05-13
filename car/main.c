#include "global.h"
// 小车运动状态
#define run_car '1'   // 按键前
#define back_car '2'  // 按键后
#define left_car '3'  // 按键左
#define right_car '4' // 按键右
#define stop_car '0'  // 按键停

void serialEvent();
void serial_data_parse_control();

void serial_data_parse_avoid();

void serial_data_parse_greenred();
void control();
void avoid();
void greenred();

int if_control = 0;
int if_avoid = 0;
int if_send = 0;
int if_face = 0;
int if_greenred = 0;

int fd_bluetooth;
int init_blue();

int init_blue()
{
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
// bluetooth control
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
void itoa(float i, char *string)
{
    sprintf(string, "%.1f", i);
    return;
}
//=============================control====================================
void control()
{
    NewLineReceived = 0;
    memset(InputString, 0x00, sizeof(InputString));
    while (if_control)
    {
        serialEvent();
        serial_data_parse_control();
        delayMicroseconds(1000);
    }
}

void serial_data_parse_control()
{
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
            puts("mingdi");
        }

        // 舵机左旋右旋判断
        if (InputString[9] == '1') // 舵机旋转到180度
        {
            puts("duoji");
        }
        if (InputString[9] == '2') // 舵机旋转到0度
        {
            puts("duoji");
        }
        // 灭火判断
        if (InputString[15] == '1') // 灭火
        {
            // TODO: 灭火
            if_control = 0;
            puts("退出手动模式");
        }

        // 舵机归为判断
        if (InputString[17] == '1') // 舵机旋转到90度
        {
            puts("duoji");
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
        NewLineReceived = 0;
        memset(InputString, 0x00, sizeof(InputString));
        return;
    }
}
//============================trace========================
volatile bool exit_program_trace = false;
volatile bool if_run_trace = false;
float getdis()
{
    float distance;
    float dis[10] = {0};
    for (int i = 0; i < 10; i++)
    {
        dis[i] = getDistance();
    }
    distance = dis[0];
    for (int i = 0; i < 10; i++)
    {
        if (dis[i] < distance)
            distance = dis[i];
    }
    // if (distance > 30)
    // {
    //     distance = 30.0;
    // }
    return distance;
}
float dis;
void serial_data_parse_trace(){
    if (StringFind((const char *)InputString, (const char *)"4WD", 0) == -1 &&
        StringFind((const char *)InputString, (const char *)"#", 0) > 0)    
    {
        // miehuo
        if (InputString[15] == '1') // 灭火
        {
            exit_program_trace = true;
            puts("执行灭火操作，退出循迹模式");
            brake();
            NewLineReceived = 0;
            memset(InputString, 0x00, sizeof(InputString));
            return;
        }
        if (InputString[3] == '1') //
        {
            if_run_trace = true;
        }
        else if (InputString[3] == '2') // 小车原地右旋
        {
            if_run_trace = false;
        }
        NewLineReceived = 0;
        memset(InputString, 0x00, sizeof(InputString));
        return;
    }
}

void *loop_thread_trace(void *arg)
{
    while (!exit_program_trace)
    {
        
        if (if_run_trace)
        {
            dis = getdis();
            if (dis < 20){
                brake();
                delayMicroseconds(5000);
            }
            
            else{
                forward();
                delayMicroseconds(5000);
            }
        }
        else{
            brake();
        }
    }
}
void *control_thread_trace(void *arg)
{
    while (!exit_program_trace)
    {
        serialEvent();
        serial_data_parse_trace();
        delayMicroseconds(1000);
    }
}
void trace()
{
    NewLineReceived = 0;
    memset(InputString, 0x00, sizeof(InputString));
    pthread_t t1, t2;
    pthread_create(&t1, NULL, loop_thread_trace, NULL);
    pthread_create(&t2, NULL, control_thread_trace, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
}

//=============================avoid====================================
volatile bool exit_program_avoid = false;
volatile bool if_run = false;
void serial_data_parse_avoid()
{
    if (StringFind((const char *)InputString, (const char *)"4WD", 0) == -1 &&
        StringFind((const char *)InputString, (const char *)"#", 0) > 0)
    {
        // miehuo
        if (InputString[15] == '1') // 灭火
        {
            exit_program_avoid = true;
            puts("执行灭火操作，退出避障模式");
            brake();
            NewLineReceived = 0;
            memset(InputString, 0x00, sizeof(InputString));
            return;
        }
        if (InputString[3] == '1') //
        {
            if_run = true;
        }
        else if (InputString[3] == '2') // 小车原地右旋
        {
            if_run = false;
        }
        NewLineReceived = 0;
        memset(InputString, 0x00, sizeof(InputString));
        return;
    }
}
void *control_thread_avoid(void *arg)
{
    while (!exit_program_avoid)
    {
        serialEvent();
        serial_data_parse_avoid();
        delayMicroseconds(1000);
    }
}

void *loop_thread_avoid(void *arg)
{
    while (!exit_program_avoid)
    {
        if (if_run)
        {
            dis = getdis();
            if (dis < 20){
                brake();
                back();
                puts("back");
                delayMicroseconds(500000);
                brake();
                delayMicroseconds(1000000);
                rotationL();
                delayMicroseconds(2000000);
                brake();
                delayMicroseconds(1000000);
            }else{
                forward();
            }
        }
        else{
            brake();

        }
    }
}
void avoid()
{
    NewLineReceived = 0;
    memset(InputString, 0x00, sizeof(InputString));
    pthread_t t1, t2;
    pthread_create(&t1, NULL, loop_thread_avoid, NULL);
    pthread_create(&t2, NULL, control_thread_avoid, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
}
//=============================trace====================================


//=============================greenred====================================
volatile bool exit_program_greenred = false;
int ri, gi, bi, li;
char *str;
void carryon()
{
    str = get_tcs34725_data();

    if (str == NULL)
    {
        printf("get_tcs34725_data 返回 NULL\n");
        return;
    }
    puts(str);
    if (sscanf(str, "R:%d,G:%d,B:%d,%d", &ri, &gi, &bi, &li) == 4)
    {
        if (ri > 160)
        {
            Move(0, 0);
            delayMicroseconds(1000);
        }
        else if (gi > 160)
        {
            Move(1, 1);
            delayMicroseconds(1000);
        }
    }
    delayMicroseconds(1000);
}

void serial_data_parse_greenred()
{
    if (StringFind((const char *)InputString, (const char *)"4WD", 0) == -1 &&
        StringFind((const char *)InputString, (const char *)"#", 0) > 0)
    {
        // miehuo
        if (InputString[15] == '1') // 灭火
        {

            exit_program_greenred = true;
            puts("执行灭火操作，退出红绿灯模式");
            brake();

            
            // 清空串口缓冲区
            NewLineReceived = 0;
            memset(InputString, 0x00, sizeof(InputString));
            return;
        }
        NewLineReceived = 0;
        memset(InputString, 0x00, sizeof(InputString));
        return;
    }
}

void *control_thread_redgreen(void *arg)
{
    while (!exit_program_greenred)
    {
        serialEvent();
        serial_data_parse_greenred();
    }
    return NULL;
}
void *loop_thread_redgreen(void *arg)
{
    while (!exit_program_greenred)
    {
        carryon();
    }
    return NULL;
}
void greenred()
{
    NewLineReceived = 0;
    memset(InputString, 0x00, sizeof(InputString));
    pthread_t t1, t2;
    pthread_create(&t1, NULL, loop_thread_redgreen, NULL);
    pthread_create(&t2, NULL, control_thread_redgreen, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
}
//=============================senddata====================================
void serial_data_postback();
volatile bool exit_program_send = false;
void serial_data_parse_send()
{
    if (StringFind((const char *)InputString, (const char *)"4WD", 0) == -1 &&
        StringFind((const char *)InputString, (const char *)"#", 0) > 0)
    {
        // miehuo
        if (InputString[15] == '1') // 灭火
        {
            exit_program_send = true;
            puts("执行灭火操作，退出发送信息模式");
            NewLineReceived = 0;
            memset(InputString, 0x00, sizeof(InputString));
            return;
        }
        NewLineReceived = 0;
        memset(InputString, 0x00, sizeof(InputString));
        return;
    }
}
void serial_data_postback();
void *loop_thread_send(void *arg)
{
    while (!exit_program_send)
    {
        serial_data_postback();
        delayMicroseconds(1000000);
    }
    printf("循环线程结束。\n");
    return NULL;
}
void *control_thread_send(void *arg)
{
    while (!exit_program_send)
    {
        serialEvent();
        serial_data_parse_send();
    }
    return NULL;
}
void senddata()
{
    NewLineReceived = 0;
    memset(InputString, 0x00, sizeof(InputString));
    pthread_t t1, t2;
    pthread_create(&t1, NULL, loop_thread_send, NULL);
    pthread_create(&t2, NULL, control_thread_send, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
}

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
                printf("→ 启动数据发送模式\n");
                exit_program_send = false;
                senddata();
                puts("退出数据发送模式");
                break;
            case 31:
                printf("→ 启动避障模式\n");
                exit_program_avoid = false;
                avoid();
                puts("退出避障模式");
                break;
            case 41:
                printf("→ 启动手动控制模式\n");
                if_control = 1;
                control();
                break;
            case 51:
                printf("→ 启动红绿灯模式\n");
                exit_program_greenred = false;
                greenred();
                break;
            case 61:
                printf("→ 启动跟随模式\n");
                exit_program_trace = false;
                trace();
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
        // miehuo
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
    distance = getdis();
    if ((int)distance == -1)
    {
        distance = 0;
    }

//#示例AT指令 : AT + SENSOR = TEMP : 25.5, PRESS : 101.3, TRACK : 01011011, AVOID : 01, COLOR : 255 - 128 - 64, DIST : 35.2
    itoa(distance, str);
    strcat(p, get_bmp280_data());
    strcat(p, ",");
    strcat(p, getTraceData());
    // 红外避障
    strcat(p, ",");
    strcat(p, getRL());
    strcat(p, ",");
    strcat(p, get_tcs34725_data());
    strcat(p, ",DIST:");
    strcat(p, str);
    // 将ReturnTemp所指的内容写入到串口设备中
    printf("ReturnTemp:%s\n", p);
    //sendTCP(p);
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
// TEST

// TEST
int main()
{
    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        wiringXGC();
        return -1;
    }

    init_blue();
    initMove();
    initTCP();
    initDistance();
    // initTCP();

    initSensor();
    init_bmp280();
    init_tcs34725();
    blink(3);
    while (1)
    {
        serial_data_postback();
        delayMicroseconds(1000000);
    }
    

    // while (1)
    // {
    //     serialEvent();
    //     serial_data_parse();
    //     delayMicroseconds(1000);
    // }
    // while (1){
    //     printf("%f\n",getDistance());
    //     delayMicroseconds(10000);
    // }
}

