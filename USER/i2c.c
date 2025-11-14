#include "i2c.h"

static I2C_Status_t I2C_Wait_Event(uint32_t event)
{
    uint32_t timeout = I2C_TIMEOUT;
    while (!I2C_CheckEvent(I2C_PORT, event))
    {
        if (--timeout == 0)
				{
						return I2C_ERR_TIMEOUT;
				}
    }
		
    return I2C_OK;
}

static I2C_Status_t I2C_Wait_Flag(FlagStatus (*flagFunc)(I2C_TypeDef*, uint32_t), uint32_t flag, FlagStatus status)
{
    uint32_t timeout = I2C_TIMEOUT;
    while (flagFunc(I2C_PORT, flag) == status)
    {
        if (--timeout == 0) 
				{
						return I2C_ERR_TIMEOUT;
				}
    }
		
    return I2C_OK;
}

static I2C_Status_t I2C_Start(void)
{
    I2C_GenerateSTART(I2C_PORT, ENABLE);
    return I2C_Wait_Event(I2C_EVENT_MASTER_MODE_SELECT);
}

static void I2C_Stop(void)
{
    I2C_GenerateSTOP(I2C_PORT, ENABLE);
}

static I2C_Status_t I2C_Send_Address(uint8_t addr, uint8_t direction)
{
    I2C_Send7bitAddress(I2C_PORT, addr << 1, direction);
    if (direction == I2C_Direction_Transmitter)
		{
				return I2C_Wait_Event(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
		}
    else
		{
				return I2C_Wait_Event(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
		}
}

static I2C_Status_t I2C_Send_Data_Wait(uint8_t data)
{
    I2C_SendData(I2C_PORT, data);
    return I2C_Wait_Event(I2C_EVENT_MASTER_BYTE_TRANSMITTED);
}

static I2C_Status_t I2C_Receive_Data_Wait(uint8_t *data)
{
    I2C_Status_t status = I2C_Wait_Event(I2C_EVENT_MASTER_BYTE_RECEIVED);
    if (status != I2C_OK) 
		{
				return status;
		}
		
    *data = I2C_ReceiveData(I2C_PORT);
		
    return I2C_OK;
}

/* ---------------------- INIT FUNCTIONS ---------------------- */
void I2C_Pins_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C_GPIO, &GPIO_InitStructure);
}

void I2C_Custom_Init(uint32_t clock_speed)
{
    I2C_InitTypeDef I2C_InitStructure = {0};
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    I2C_InitStructure.I2C_ClockSpeed = clock_speed;
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C_PORT, &I2C_InitStructure);

    I2C_Cmd(I2C_PORT, ENABLE);
}

I2C_Status_t I2C_Scan_Address(void)
{
    uint8_t addr;
    uint8_t found = 0;

    for (addr = 1; addr < 128; addr++)
    {
        if (I2C_Wait_Flag(I2C_GetFlagStatus, I2C_FLAG_BUSY, SET) != I2C_OK)
        {
            I2C_Stop();
            continue;
        }

        // START condition
        if (I2C_Start() != I2C_OK)
        {
            I2C_Stop();
            continue;
        }

        if (I2C_Send_Address(addr, I2C_Direction_Transmitter) == I2C_OK)
        {
            printf("Found device at 0x%02X\r\n", addr);
            found = 1;
        }

        I2C_Stop();
        SysTick_Driver.Delay_Ms(5);
    }

    return (found ? I2C_OK : I2C_ERR_TIMEOUT);
}

/* ---------------------- HIGH-LEVEL API ---------------------- */
I2C_Status_t I2C_Write_Byte(uint8_t devAddr, uint8_t regAddr, uint8_t data)
{
    if (I2C_Wait_Flag(I2C_GetFlagStatus, I2C_FLAG_BUSY, SET) != I2C_OK) 
		{
				return I2C_ERR_BUSY;
		}

    if (I2C_Start() != I2C_OK) 
		{
				return I2C_ERR_START;
		}
		
    if (I2C_Send_Address(devAddr, I2C_Direction_Transmitter) != I2C_OK)
		{
				return I2C_ERR_ADDR;
		}

    if (I2C_Send_Data_Wait(regAddr) != I2C_OK) 
		{
				return I2C_ERR_TXE;
		}
			
    if (I2C_Send_Data_Wait(data) != I2C_OK) 
		{
				return I2C_ERR_TXE;
		}

    I2C_Stop();
		
    return I2C_OK;
}

I2C_Status_t I2C_Write_Bytes(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len)
{
    if (I2C_Wait_Flag(I2C_GetFlagStatus, I2C_FLAG_BUSY, SET) != I2C_OK) 
		{
				return I2C_ERR_BUSY;
		}

    if (I2C_Start() != I2C_OK) 
		{
				return I2C_ERR_START;
		}
		
    if (I2C_Send_Address(devAddr, I2C_Direction_Transmitter) != I2C_OK) 
		{
				return I2C_ERR_ADDR;
		}

    if (I2C_Send_Data_Wait(regAddr) != I2C_OK) 
		{
				return I2C_ERR_TXE;
		}

    for (uint8_t i = 0; i < len; i++)
		{
        if (I2C_Send_Data_Wait(data[i]) != I2C_OK) 
				{
						return I2C_ERR_TXE;
				}
		}
		
    I2C_Stop();
		
    return I2C_OK;
}

I2C_Status_t I2C_Read_Byte(uint8_t devAddr, uint8_t regAddr, uint8_t *data)
{
    if (I2C_Wait_Flag(I2C_GetFlagStatus, I2C_FLAG_BUSY, SET) != I2C_OK) 
		{
				return I2C_ERR_BUSY;
		}

    // Write register address
    if (I2C_Start() != I2C_OK) 
		{
				return I2C_ERR_START;
		}
		
    if (I2C_Send_Address(devAddr, I2C_Direction_Transmitter) != I2C_OK) 
		{
				return I2C_ERR_ADDR;
		}
		
    if (I2C_Send_Data_Wait(regAddr) != I2C_OK) 
		{
				return I2C_ERR_TXE;
		}

    // Restart & read
    if (I2C_Start() != I2C_OK) 
		{
				return I2C_ERR_START;
		}
		
    if (I2C_Send_Address(devAddr, I2C_Direction_Receiver) != I2C_OK) 
		{
				return I2C_ERR_ADDR;
		}

    I2C_AcknowledgeConfig(I2C_PORT, DISABLE); // Single byte => NACK
		
    I2C_Status_t status = I2C_Receive_Data_Wait(data);
    I2C_Stop();
		
    I2C_AcknowledgeConfig(I2C_PORT, ENABLE);
		
    return status;
}

I2C_Status_t I2C_Read_Bytes(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len)
{
    if (I2C_Wait_Flag(I2C_GetFlagStatus, I2C_FLAG_BUSY, SET) != I2C_OK) 
		{
				return I2C_ERR_BUSY;
		}

    // Write register address
    if (I2C_Start() != I2C_OK) 
		{
				return I2C_ERR_START;
		}
		
    if (I2C_Send_Address(devAddr, I2C_Direction_Transmitter) != I2C_OK) 
		{
				return I2C_ERR_ADDR;
		}
		
    if (I2C_Send_Data_Wait(regAddr) != I2C_OK) 
		{
				return I2C_ERR_TXE;
		}

    // Restart & read
    if (I2C_Start() != I2C_OK) 
		{
				return I2C_ERR_START;
		}
		
    if (I2C_Send_Address(devAddr, I2C_Direction_Receiver) != I2C_OK) 
		{
				return I2C_ERR_ADDR;
		}

    for (uint8_t i = 0; i < len; i++)
    {
        if (i == len - 1)
        {
            I2C_AcknowledgeConfig(I2C_PORT, DISABLE); // Last byte
            I2C_Stop();
        }
				
        if (I2C_Receive_Data_Wait(&data[i]) != I2C_OK) 
				{
						return I2C_ERR_RXE;
				}
    }

    I2C_AcknowledgeConfig(I2C_PORT, ENABLE);
		
    return I2C_OK;
}
