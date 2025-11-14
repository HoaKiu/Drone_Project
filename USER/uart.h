#ifndef __UART_H__
#define __UART_H__

#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART

#ifdef __cplusplus
extern "C"{
#endif

#define NULL 0

#define UART_PORT   GPIOA
#define UART_TX_PIN GPIO_Pin_9
#define UART_RX_PIN GPIO_Pin_10

typedef enum 
{
    UART1_INDEX = 0,
    UART2_INDEX,
    UART3_INDEX,
    UART_MAX
} 
UART_Index_t;

typedef struct 
{
    void (*Pins_Init)(void);
    void (*Init)(USART_TypeDef*, const USART_InitTypeDef*);
    void (*Enable_IRQ)(USART_TypeDef*, uint16_t, uint32_t, uint8_t, uint8_t);
	
    void (*Send_Char)(USART_TypeDef*, const char);
    void (*Send_String)(USART_TypeDef*, const char*);
    void (*Send_Line)(USART_TypeDef*, const char*);
		uint8_t (*Receive_Char)(USART_TypeDef*);
	
		void (*Set_Flag)(UART_Index_t , uint16_t);
		void (*Clear_Flag)(UART_Index_t , uint16_t);
		_Bool (*Get_Flag)(UART_Index_t , uint16_t);
} 
UART_Driver_t;

extern const UART_Driver_t UART_Driver;
extern const USART_InitTypeDef UARTx_Config[UART_MAX];

#ifdef __cplusplus
}
#endif

#endif /* __UART_H__ */
