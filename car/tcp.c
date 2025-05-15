#include "shin.h"
#include "tcp.h"
int fd_tcp = -1;
int fd_receive = -1;
int waitForResponse(const char *expected, int timeout_ms);
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
    wiringXSerialPrintf(fd_tcp, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", PC_IP, PORT);
    sleep(1);
    wiringXSerialPrintf(fd_tcp, "AT+CIPSTATUS\r\n");
    wiringXSerialPrintf(fd_tcp, "AT+CIPSTATUS\r\n");
    if (!waitForResponse("STATUS:3", 1000))
    { // 3表示已连接
        printf("连接状态异常\n");
        return -1;
    }
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
    wiringXSerialPrintf(fd_tcp, "AT+CIPSTATUS\r\n");
    if (!waitForResponse("STATUS:3", 1000))
    {
        printf("连接已断开，尝试重连...\n");
        if (initTCP() != 0)
        {
            printf("重连失败\n");
            return;
        }
    }
        int data_len = strlen(data);
        // 2. 请求发送权限
        wiringXSerialPrintf(fd_tcp, "AT+CIPSEND=%d\r\n", data_len);
        if (!waitForResponse(">", 1000))
        {
            printf("未收到发送提示符\n");
            return;
        }
        // 3. 正文数据发送
        wiringXSerialPrintf(fd_tcp, "%s%c", data, 0x1A);
        if (waitForResponse("SEND OK", 2000))
        {
            printf("发送成功\n");
        }
        else
        {
            printf("发送失败\n");
        }

        return;
}
