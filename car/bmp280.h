#ifndef BMP280_H
#define BMP280_H
#include "shin.h"
#define I2C_ADDR 0x77

// hardware registers
#define REG_CONFIG 0xF5
#define REG_CTRL_MEAS 0xF4
#define REG_RESET 0xE0

#define REG_TEMP_XLSB 0xFC
#define REG_TEMP_LSB 0xFB
#define REG_TEMP_MSB 0xFA

#define REG_PRESSURE_XLSB 0xF9
#define REG_PRESSURE_LSB 0xF8
#define REG_PRESSURE_MSB 0xF7

// calibration registers
#define REG_DIG_T1 0x88
#define REG_DIG_T2 0x8A
#define REG_DIG_T3 0x8C

#define REG_DIG_P1 0x8E
#define REG_DIG_P2 0x90
#define REG_DIG_P3 0x92
#define REG_DIG_P4 0x94
#define REG_DIG_P5 0x96
#define REG_DIG_P6 0x98
#define REG_DIG_P7 0x9A
#define REG_DIG_P8 0x9C
#define REG_DIG_P9 0x9E
struct bmp280_calib_param
{
    // temperature params
    uint16_t dig_t1;
    int16_t dig_t2;
    int16_t dig_t3;

    // pressure params
    uint16_t dig_p1;
    int16_t dig_p2;
    int16_t dig_p3;
    int16_t dig_p4;
    int16_t dig_p5;
    int16_t dig_p6;
    int16_t dig_p7;
    int16_t dig_p8;
    int16_t dig_p9;
};
void bmp280_read_raw(int fd, int32_t* temp, int32_t* pressure);
void bmp280_init(int fd);
void bmp280_get_calib_params(int fd, struct bmp280_calib_param *params);
int32_t bmp280_convert(int32_t temp, struct bmp280_calib_param *params);
int32_t bmp280_convert_temp(int32_t temp, struct bmp280_calib_param *params);
int32_t bmp280_convert_pressure(int32_t pressure, int32_t temp, struct bmp280_calib_param *params);
int init_bmp280();
char* get_bmp280_data();
#endif
