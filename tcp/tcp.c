#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "wiringx.h"

#define SERIAL_DEV "/dev/ttyS2"
#define PC_IP "192.168.80.147"
#define PORT 8080
#define SSID "ChenMagic6"
#define PASSWORD "88888888"
int fd_receive;
int fd_send;
int initTCP(){
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
    fd_receive = wiringXSerialOpen(SERIAL_DEV, serial_config);
    fd_send = fd_receive;
    if (fd_send < 0)
    {
        perror("串口打开失败");
        return 1;
    }
    printf("串口打开成功，开始接收数据...\n");
    wiringXSerialFlush(fd_send);
    wiringXSerialPrintf(fd_send, "AT+RST\r\n");
    sleep(5);
    // 修正AT指令格式
    wiringXSerialPrintf(fd_send, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", PC_IP, PORT);
    sleep(5);
    return 0;
};
int sensor_test = 0;
char *read_sensor()
{
    static char buffer[80]; // 使用静态变量
    snprintf(buffer, sizeof(buffer), "sensor data test %d", sensor_test);
    return buffer;
}
void execute_command(const char *cmd)
{
    printf("Executing command: %s\n", cmd);
}
void *receiver_thread(void *arg)
{
    int count = 0;
    while(1){
        count = wiringXSerialDataAvail(fd_receive);
        if (count > 0)
        {
            while (count--)
            {
                char ch = wiringXSerialGetChar(fd_receive);
                printf("%c", ch);
                fflush(stdout);
            }
        }
    }
    return NULL;
}

void send_data(const char *data)
{
    int len = strlen(data);
    wiringXSerialPrintf(fd_send, "AT+CIPSEND=%d\r\n", len);
    delayMicroseconds(100000);
    wiringXSerialPrintf(fd_send, "%s%c", data, 0x1A);
    delayMicroseconds(100000);
}

int main(){
    if(initTCP()<0){
        printf("tcp init failed\n");
        return -1;
    }
    int a = 5;

    pthread_t tid_recv;
    pthread_create(&tid_recv, NULL, receiver_thread, NULL);
    while (1)
    {
        send_data(read_sensor());
        sensor_test++;
        sleep(1);
    }
    pthread_join(tid_recv, NULL);
    return 0;
}
