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
    wiringXSerialPrintf(fd_tcp, "AT+RST\r\n");
    sleep(5);
    // 修正AT指令格式
    wiringXSerialPrintf(fd_tcp, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", PC_IP, PORT);
    sleep(5);
    return 0;
};
void sendTCP(char *data){
    int len = strlen(data);
    wiringXSerialPrintf(fd_tcp, "AT+CIPSEND=%d\r\n", len);
    delayMicroseconds(50000);
    wiringXSerialPrintf(fd_tcp, "%s%c", data, 0x1A);
    delayMicroseconds(50000);
}
