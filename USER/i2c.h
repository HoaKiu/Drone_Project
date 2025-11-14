#ifndef  __I2C_H__
#define __I2C_H__

#include "stm32f10x.h"                  // Device header
#include "systick.h"
#include "printf_redirect.h"

#define I2C_PORT        I2C1
#define I2C_GPIO        GPIOB
#define I2C_SCL_PIN     GPIO_Pin_6
#define I2C_SDA_PIN     GPIO_Pin_7

#define I2C_TIMEOUT     10000U
#define I2C_CLOCK_SPEED 100000U

typedef enum 
{
    I2C_OK = 0,
    I2C_ERR_BUSY,
    I2C_ERR_START,
    I2C_ERR_ADDR,
    I2C_ERR_TXE,
    I2C_ERR_RXE,
    I2C_ERR_TIMEOUT
} I2C_Status_t;

void I2C_Pins_Init(void);
void I2C_Custom_Init(uint32_t clock_speed);

I2C_Status_t I2C_Scan_Address(void);
I2C_Status_t I2C_Write_Byte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
I2C_Status_t I2C_Read_Byte(uint8_t devAddr, uint8_t regAddr, uint8_t *data);
I2C_Status_t I2C_Read_Bytes(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len);
I2C_Status_t I2C_Write_Bytes(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len);

#endif /* __I2C_H__ */
