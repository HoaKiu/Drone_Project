#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "stm32f10x.h"
#include "i2c.h"
#include "systick.h"
#include "printf_redirect.h"
#include <math.h>

/* ---------------- MPU6050 I2C ADDRESS ---------------- */
#define MPU_ADDR            0x68

/* ---------------- MPU6050 REGISTER MAP ---------------- */
#define REG_SMPLRT_DIV      0x19
#define REG_CONFIG          0x1A
#define REG_GYRO_CONFIG     0x1B
#define REG_ACCEL_CONFIG    0x1C

#define REG_ACCEL_XOUT_H    0x3B
#define REG_GYRO_XOUT_H     0x43

#define REG_PWR_MGMT_1      0x6B
#define REG_WHO_AM_I        0x75

/* ---------------- FUNCTION PROTOTYPES ---------------- */
void MPU6050_Init(void);

void MPU6050_Read_Accel(int16_t *Ax, int16_t *Ay, int16_t *Az);
void MPU6050_Read_Gyro(int16_t *Gx, int16_t *Gy, int16_t *Gz);

void MPU6050_Calibrate(uint16_t samples);

void MPU6050_Read_Accel_Calibrated(float *Ax, float *Ay, float *Az);
void MPU6050_Read_Gyro_Calibrated(float *Gx, float *Gy, float *Gz);

void MPU6050_Get_Pitch(float *pitch);
void MPU6050_Get_Roll(float *roll);

#endif
