#include "mpu6050.h"

/* ---------------- OFFSET VALUES ---------------- */
static int16_t AccX_offset = 0;
static int16_t AccY_offset = 0;
static int16_t AccZ_offset = 0;

static int16_t GyroX_offset = 0;
static int16_t GyroY_offset = 0;
static int16_t GyroZ_offset = 0;

uint8_t whoami = 0;

void MPU6050_Init(void)
{
    I2C_Pins_Init();
    I2C_Custom_Init(I2C_CLOCK_SPEED);

    // Ð?c WHO_AM_I
    if (I2C_Read_Byte(MPU_ADDR, REG_WHO_AM_I, &whoami) != I2C_OK)
    {
        printf("I2C READ FAIL!\r\n");
        return;
    }

    if (whoami != 0x68)
    {
        printf("MPU6050 NOT FOUND!\r\n");
        return;
    }

    // Wake up MPU6050
    I2C_Write_Byte(MPU_ADDR, REG_PWR_MGMT_1, 0x00);
    SysTick_Driver.Delay_Ms(10);

    I2C_Write_Byte(MPU_ADDR, REG_SMPLRT_DIV, 0x07);   // 1kHz / (7+1) = 125Hz
    I2C_Write_Byte(MPU_ADDR, REG_CONFIG, 0x00);       // No DLPF
    I2C_Write_Byte(MPU_ADDR, REG_GYRO_CONFIG, 0x00);  // ±250°/s
    I2C_Write_Byte(MPU_ADDR, REG_ACCEL_CONFIG, 0x00); // ±2g

    printf("MPU6050 INIT OK\r\n");
}

/* ---------------------------------------------------- */
void MPU6050_Read_Accel(int16_t *Ax, int16_t *Ay, int16_t *Az)
{
    uint8_t buf[6];

    if (I2C_Read_Bytes(MPU_ADDR, REG_ACCEL_XOUT_H, buf, 6) != I2C_OK)
        return;

    *Ax = (int16_t)(buf[0] << 8 | buf[1]);
    *Ay = (int16_t)(buf[2] << 8 | buf[3]);
    *Az = (int16_t)(buf[4] << 8 | buf[5]);
}

/* ---------------------------------------------------- */
void MPU6050_Read_Gyro(int16_t *Gx, int16_t *Gy, int16_t *Gz)
{
    uint8_t buf[6];

    if (I2C_Read_Bytes(MPU_ADDR, REG_GYRO_XOUT_H, buf, 6) != I2C_OK)
        return;

    *Gx = (int16_t)(buf[0] << 8 | buf[1]);
    *Gy = (int16_t)(buf[2] << 8 | buf[3]);
    *Gz = (int16_t)(buf[4] << 8 | buf[5]);
}

/* ---------------------------------------------------- */
void MPU6050_Calibrate(uint16_t samples)
{
    int32_t sumAx = 0, sumAy = 0, sumAz = 0;
    int32_t sumGx = 0, sumGy = 0, sumGz = 0;

    int16_t ax, ay, az;
    int16_t gx, gy, gz;

    for (uint16_t i = 0; i < samples; i++)
    {
        MPU6050_Read_Accel(&ax, &ay, &az);
        MPU6050_Read_Gyro(&gx, &gy, &gz);

        sumAx += ax;
        sumAy += ay;
        sumAz += (az - 16384); // tr? 1g

        sumGx += gx;
        sumGy += gy;
        sumGz += gz;

        SysTick_Driver.Delay_Ms(2);
    }

    AccX_offset = sumAx / samples;
    AccY_offset = sumAy / samples;
    AccZ_offset = sumAz / samples;

    GyroX_offset = sumGx / samples;
    GyroY_offset = sumGy / samples;
    GyroZ_offset = sumGz / samples;
}

/* ---------------------------------------------------- */
void MPU6050_Read_Accel_Calibrated(float *Ax, float *Ay, float *Az)
{
    int16_t rx, ry, rz;
    MPU6050_Read_Accel(&rx, &ry, &rz);

    *Ax = (rx - AccX_offset) / 16384.0f;
    *Ay = (ry - AccY_offset) / 16384.0f;
    *Az = (rz - AccZ_offset) / 16384.0f;
}

/* ---------------------------------------------------- */
void MPU6050_Read_Gyro_Calibrated(float *Gx, float *Gy, float *Gz)
{
    int16_t rx, ry, rz;
    MPU6050_Read_Gyro(&rx, &ry, &rz);

    *Gx = (rx - GyroX_offset) / 131.0f;
    *Gy = (ry - GyroY_offset) / 131.0f;
    *Gz = (rz - GyroZ_offset) / 131.0f;
}

void MPU6050_Get_Pitch(float *pitch)
{
    float Ax, Ay, Az;
    MPU6050_Read_Accel_Calibrated(&Ax, &Ay, &Az);

    // Pitch: nghiêng v? tru?c / sau
    *pitch = atan2f(Ax, sqrtf(Ay*Ay + Az*Az)) * 180.0f / 3.14159265f;
}

void MPU6050_Get_Roll(float *roll)
{
    float Ax, Ay, Az;
    MPU6050_Read_Accel_Calibrated(&Ax, &Ay, &Az);

    // Roll: nghiêng sang trái / ph?i
    *roll  = atan2f(Ay, sqrtf(Ax*Ax + Az*Az)) * 180.0f / 3.14159265f;
}