#include "shin.h"
#include "tcp.h"
int fd_tcp = -1;
int fd_receive = -1;
int initTCP()
{
    if (wiringXSetup("milkv_duo", NULL) == -1)
    {
        printf("WiringX初始化失败！\n");
        return -1;
    }
    puts("tcp初始化开始");
    wiringXSerial_t serial_config;
    serial_config.baud = 115200;
    serial_config.databits = 8;
    serial_config.parity = 'n'; // 无校验
    serial_config.stopbits = 1;
    serial_config.flowcontrol = 'n'; // 无流控
    fd_receive = wiringXSerialOpen(UART_NET, serial_config);
    fd_tcp = fd_receive;
    if (fd_tcp < 0)
    {
        perror("串口打开失败");
        return -1;
    }
    printf("串口打开成功，开始接收数据...\n");
    wiringXSerialFlush(fd_tcp);
    // 修正AT指令格式

    sendTCP("TEMP : 25.5, PRESS : 101.3, TRACK : 01011011, AVOID : 01, COLOR : 255 - 128 - 64, DIST : 35.2");
    wiringXSerialPrintf(fd_tcp, "AT+CIPCLOSE\r\n");
    sleep(2);
    wiringXSerialPrintf(fd_tcp, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", PC_IP, PORT);
    sleep(3);
    puts("tcp初始化结束");
    return 0;
};
unsigned long getCurrentMillis() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
int waitForResponse(const char *expected, int timeout_ms)
{
    char buffer[256] = {0};
    int idx = 0;
    uint64_t start = getCurrentMillis();

    while ((getCurrentMillis() - start) < timeout_ms)
    {
        if (wiringXSerialDataAvail(fd_tcp) > 0)
        {
            char c = wiringXSerialGetChar(fd_tcp);
            if (idx < sizeof(buffer) - 1)
                buffer[idx++] = c;
            buffer[idx] = '\0';
            if (strstr(buffer, expected))
                return 1;
        }
        delayMicroseconds(1000);
    }
    return 0;
}
void sendTCP(char *data){
    // int len = strlen(data);
    // wiringXSerialPrintf(fd_tcp, "AT+CIPSEND=%d\r\n", len);
    // delayMicroseconds(500000);
    // wiringXSerialPrintf(fd_tcp, "%s%c", data, 0x1A);
    // delayMicroseconds(500000);

    // char http_request[512];
    // int data_len = strlen(data);

    // snprintf(http_request, sizeof(http_request),
    //          "POST /api/sensor-data HTTP/1.1\r\n"
    //          "Host: %s:%d\r\n"
    //          "Content-Type: text/plain\r\n"
    //          "Content-Length: %d\r\n"
    //          "Connection: close\r\n"
    //          "\r\n"
    //          "%s",
    //          PC_IP, PORT, data_len, data);

    // int total_len = strlen(http_request);
    // // 1. 发送数据长度
    // wiringXSerialPrintf(fd_tcp, "AT+CIPSEND=%d\r\n", total_len);
    // uint64_t start = getCurrentMillis();
    // while ((getCurrentMillis() - start) < 1000)
    // {
    //     if (wiringXSerialDataAvail(fd_tcp) > 0)
    //     {
    //         char c = wiringXSerialGetChar(fd_tcp);
            
    //         if (c == '>')
    //             break; // 收到提示符
    //     }
    //     delayMicroseconds(1000);
    // }
    // wiringXSerialPrintf(fd_tcp, "%s%c", http_request, 0x1A);
    // wiringXSerialPrintf(fd_tcp, "AT+CIPCLOSE\r\n");
    // printf("[TCP] 数据发送成功\n");
    // sleep(2);
    // wiringXSerialPrintf(fd_tcp, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", PC_IP, PORT);
    // sleep(3);
    // return ;
        char http_request[512];
        int data_len = strlen(data);

        snprintf(http_request, sizeof(http_request),
                 "POST /api/sensor-data HTTP/1.1\r\n"
                 "Host: %s:%d\r\n"
                 "Content-Type: text/plain\r\n"
                 "Content-Length: %d\r\n"
                 "Connection: close\r\n"
                 "\r\n"
                 "%s",
                 PC_IP, PORT, data_len, data);

        int total_len = strlen(http_request);

        // 1. 启动连接
        wiringXSerialPrintf(fd_tcp, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", PC_IP, PORT);
        if (!waitForResponse("OK", 3000))
        {
            printf("启动TCP连接失败\n");
            return;
        }

        // 2. 请求发送权限
        wiringXSerialPrintf(fd_tcp, "AT+CIPSEND=%d\r\n", total_len);
        if (!waitForResponse(">", 2000))
        {
            printf("未收到发送提示符\n");
            return;
        }

        // 3. 正文数据发送
        wiringXSerialPrintf(fd_tcp, "%s%c", http_request, 0x1A);
        if (!waitForResponse("SEND OK", 3000))
        {
            printf("发送失败\n");
            return;
        }

        // 4. 主动关闭连接
        wiringXSerialPrintf(fd_tcp, "AT+CIPCLOSE\r\n");
        waitForResponse("CLOSED", 2000);

        printf("[TCP] 数据发送成功\n");
        return;
}
