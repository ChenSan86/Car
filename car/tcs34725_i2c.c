#include "shin.h"
#include "tcs34725_i2c.h"

int fd_rgb;
char *ReturnTemp_tcs;

static bool tcs34725_state = false;

struct tcs34725_config _tcs34725_config =
    {
        .name = "TCS34725",
        .interrupt_start = false,
};
static uint8_t tcs34725_write8(uint8_t reg_addr, uint8_t write_data)
{
    return wiringXI2CWriteReg8(fd_rgb, TCS34725_COMMAND_BIT | reg_addr, write_data);
}

/**
 * @brief 从寄存器读一字节 Read one byte from register
 *
 * @param reg_addr 寄存器地址 Register address
 * @param read_data 数据存放地址 Point to the data storage address
 * @return uint8_t 无错误时返回 0 Returns 0 if there is no error
 */
static uint8_t tcs34725_read8(uint8_t reg_addr, uint8_t *read_data)
{
    *read_data = wiringXI2CReadReg8(fd_rgb, TCS34725_COMMAND_BIT | reg_addr);
    return 0;
}

/**
 * @brief 从寄存器读两字节 Read two byte from register
 *
 * @param reg_addr 寄存器地址 Register address
 * @param read_data 数据存放地址 Point to the data storage address
 * @return uint8_t 无错误时返回 0 Returns 0 if there is no error
 */
static uint8_t tcs34725_read16(uint8_t reg_addr, uint16_t *read_data)
{
    *read_data = wiringXI2CReadReg16(fd_rgb, TCS34725_COMMAND_BIT | reg_addr);
    return 0;
}

/**
 * @brief 启动 tcs34725 设备 Start the tcs34725 device
 *
 * @param interrupt_start 是否开启中断启动方式 Whether to enable interrupt startup mode
 * @return uint8_t 无错误时返回 0 Returns 0 if there is no error
 */
uint8_t tcs34725_start(bool interrupt_start)
{
    int err = 0;
    if (tcs34725_state)
    {
        return -1;
    }

    err = tcs34725_write8(TCS34725_ENABLE, TCS34725_ENABLE_PON);
    err = tcs34725_write8(TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);

    if (interrupt_start)
    {
        _tcs34725_config.interrupt_start = interrupt_start;
        err = tcs34725_write8(TCS34725_ENABLE, TCS34725_ENABLE_AIEN);
    }

    tcs34725_state = true;
    return err;
}

/**
 * @brief 停止 tcs34725 设备 Stop tcs34725 device
 *
 * @return uint8_t 无错误时返回 0 Returns 0 if there is no error
 */
uint8_t tcs34725_stop(void)
{
    uint8_t err = 0;
    uint8_t data = 0;
    err = tcs34725_read8(TCS34725_ENABLE, &data);
    err = tcs34725_write8(TCS34725_ENABLE, data & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN));

    if (_tcs34725_config.interrupt_start)
    {
        err = tcs34725_write8(TCS34725_ENABLE, data & ~TCS34725_ENABLE_AIEN);
    }

    tcs34725_state = false;
    return err;
}

/**
 * @brief 读取设备类型 Read device type
 *
 * @return uint8_t 设备识别号 Device identification number 0x44 = TCS34721/TCS34725, 0x4D = TCS34723/TCS34727
 */
uint8_t get_tcs34725_type(void)
{
    uint8_t data = 0;
    tcs34725_read8(TCS34725_ID, &data);
    return data;
}

/**
 * @brief 设置集成时间 Set integration time
 *
 * @param integration_time 集成时间 integration time
 * @return uint8_t 无错误时返回 0 Returns 0 if there is no error
 */
uint8_t set_tcs34725_integration_time(tcs34725_integration_time_t integration_time)
{
    _tcs34725_config.integration_time = integration_time;
    return tcs34725_write8(TCS34725_ATIME, integration_time);
}

/**
 * @brief 获取设置的集成时间 Get the set integration time
 *
 * @return 集成时间配置参数 Integration time configuration parameters
 */
tcs34725_integration_time_t get_tcs34725_integration_time(void)
{
    return _tcs34725_config.integration_time;
}

/**
 * @brief 设置增益倍数 Set gain multiplier
 *
 * @param gain 增益倍数 Gain multiple
 * @return uint8_t 无错误时返回 0 Returns 0 if there is no error
 */
uint8_t set_tcs34725_gain(tcs34725_gain_t gain)
{
    _tcs34725_config.gain = gain;
    return tcs34725_write8(TCS34725_CONTROL, gain);
}

/**
 * @brief 获取设置的增益倍数 Get the set gain multiplier
 *
 * @return 增益倍数 Gain multiple
 */
tcs34725_gain_t get_tcs34725_gain(void)
{
    return _tcs34725_config.gain;
}

/**
 * @brief 获取RGBC的值 Get the value of RGBC
 *
 * @param colour_r 红色数据存放地址 Red data storage address
 * @param colour_g 绿色数据存放地址 Green data storage address
 * @param colour_b 蓝色数据存放地址 Blue data storage address
 * @param colour_c 亮度数据存放地址 Brightness data storage address
 * @return uint8_t 无错误时返回 0 Returns 0 if there is no error
 */
uint8_t get_tcs34725_rgbc(uint16_t *colour_r, uint16_t *colour_g, uint16_t *colour_b, uint16_t *colour_c)
{
    uint8_t err = 0;
    err = tcs34725_read16(TCS34725_RDATAL, colour_r);
    err = tcs34725_read16(TCS34725_GDATAL, colour_g);
    err = tcs34725_read16(TCS34725_BDATAL, colour_b);
    err = tcs34725_read16(TCS34725_CDATAL, colour_c);
    return err;
}
uint16_t r, g, b, l;
uint8_t tcs34725_type;
int init_tcs34725()
{
    if ((fd_rgb = wiringXI2CSetup(I2C_RGB, TCS34725_ADDR)) < 0)
    {
        printf("I2C Setup failed: %i\n", fd_rgb);
        return -1;
    }

    tcs34725_type = get_tcs34725_type();
    printf("device type is : %x \n", tcs34725_type);
    if (!((tcs34725_type == 0x44) || (tcs34725_type == 0x4D)))
    {
        printf("Wrong device type! exit\n");
        return -1;
    }

    set_tcs34725_integration_time(TCS34725_INTEGRATIONTIME_50MS);
    set_tcs34725_gain(TCS34725_GAIN_4X);
    tcs34725_start(0);
    ReturnTemp_tcs = (char*)malloc(50);
    delayMicroseconds(100000);
    return 0;
}
char* get_tcs34725_data(){
    get_tcs34725_rgbc(&r, &g, &b, &l);
    sprintf(ReturnTemp_tcs,"R:%d,G:%d,B:%d,%d",r,g,b,l);
    return ReturnTemp_tcs;
}
