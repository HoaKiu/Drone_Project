#include "systick.h"
#include "uart.h"
#include "printf_redirect.h"
#include "mpu6050.h"

int16_t Ax_raw, Ay_raw, Az_raw;
int16_t Gx_raw, Gy_raw, Gz_raw;

float Ax, Ay, Az;
float Gx, Gy, Gz;

float pitch, roll;

int main(void)
{
		SystemInit();
		SysTick_Driver.Init();
		UART_Driver.Pins_Init();
		UART_Driver.Init(USART1, &UARTx_Config[UART1_INDEX]);
	
    printf("===== DRONE=====\r\n");

    MPU6050_Init();  
    SysTick_Driver.Delay_Ms(100);

    printf("Calibrating... Keep MPU6050 still\r\n");
    MPU6050_Calibrate(500);
    printf("Calibration done!\r\n");

    while (1)
    {
				MPU6050_Get_Pitch(&pitch);
				MPU6050_Get_Roll(&roll);
        /* Read calibrated */
        MPU6050_Read_Accel_Calibrated(&Ax, &Ay, &Az);
        MPU6050_Read_Gyro_Calibrated(&Gx, &Gy, &Gz);

        printf("AX=%.3f AY=%.3f AZ=%.3f | GX=%.3f GY=%.3f GZ=%.3f\r\n",
                Ax, Ay, Az,
                Gx, Gy, Gz);
				
				printf("Pitch=%.2f° | Roll=%.2f°\r\n", pitch, roll);
			
        SysTick_Driver.Delay_Ms(1000);
    }
}
