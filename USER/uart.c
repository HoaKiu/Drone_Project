#include "uart.h"

static volatile uint16_t uart_flag[UART_MAX] = {0};

// prototype
static void UART_Enable_Clock(USART_TypeDef* USARTx);
static void GPIO_Enable_Clock(GPIO_TypeDef* GPIOx);

static void UART_Pins_Init(void);
static void UARTx_Init(USART_TypeDef* UARTx, const USART_InitTypeDef* UARTx_Config);

static void UART_Enable_IRQ(USART_TypeDef* USARTx, uint16_t USART_IT, uint32_t NVIC_PriorityGroup, \
                     uint8_t PreemptionPriority, uint8_t SubPriority);

static void UART_Send_Char(USART_TypeDef* USARTx, const char ch);
static void UART_Send_String(USART_TypeDef* USARTx, const char* str);
static void UART_Send_Line(USART_TypeDef* USARTx, const char* str);
static uint8_t UART_Receive_Char(USART_TypeDef* USARTx);

static void UART_Set_Flag(UART_Index_t uart_index, uint16_t flag);
static void UART_Clear_Flag(UART_Index_t uart_index, uint16_t flag);
static _Bool UART_Get_Flag(UART_Index_t uart_index, uint16_t flag);

static void UART_Enable_Clock(USART_TypeDef* USARTx)
{
		if (USARTx == USART1)
		{
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		}
		else if (USARTx == USART2)
		{
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		}
		else if (USARTx == USART3)
		{
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		}
		else
		{
				return;
		}
}

static void GPIO_Enable_Clock(GPIO_TypeDef* GPIOx)
{
    if (GPIOx == GPIOA)
		{
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		}
    else if (GPIOx == GPIOB) 
		{
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		}
    else if (GPIOx == GPIOC) 
		{
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		}
		else 
		{
				return;
		}
}

static void UART_Pins_Init(void)
{
		GPIO_Enable_Clock(UART_PORT);		
		GPIO_InitTypeDef GPIO_InitStructure = {0};
		
		// TX
		GPIO_InitStructure.GPIO_Pin = UART_TX_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(UART_PORT, &GPIO_InitStructure);
		
		// RX
		GPIO_InitStructure.GPIO_Pin = UART_RX_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(UART_PORT, &GPIO_InitStructure);
}

static void UARTx_Init(USART_TypeDef* USARTx, const USART_InitTypeDef* UARTx_Config)
{
		if (UARTx_Config == NULL)
		{
				return;
		}
		
		UART_Enable_Clock(USARTx);
		
		USART_InitTypeDef UART_InitStructure = *UARTx_Config;
		USART_Init(USARTx, &UART_InitStructure);
		
		USART_Cmd(USARTx, ENABLE);
}

static void UART_Enable_IRQ(USART_TypeDef* USARTx, uint16_t USART_IT, uint32_t NVIC_PriorityGroup, \
                     uint8_t PreemptionPriority, uint8_t SubPriority)
{
		NVIC_InitTypeDef NVIC_InitStructure = {0};
		
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup);
	
    if (USARTx == USART1)
		{
				NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		}
    else if (USARTx == USART2)
		{
				NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		}
    else if (USARTx == USART3)
		{
				NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		}
    else
		{
				return;
		}
		
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USARTx, USART_IT, ENABLE);
}

static void UART_Send_Char(USART_TypeDef* USARTx, const char ch)
{
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    USART_SendData(USARTx, (uint16_t)ch);
}

static void UART_Send_String(USART_TypeDef* USARTx, const char* str)
{
    while (*str)
    {
        UART_Send_Char(USARTx, *str++);
    }
}

static void UART_Send_Line(USART_TypeDef* USARTx, const char* str)
{
    UART_Send_String(USARTx, str);
    UART_Send_String(USARTx, "\r\n");
}

static uint8_t UART_Receive_Char(USART_TypeDef* USARTx)
{
    return (uint8_t)USART_ReceiveData(USARTx);
}

static void UART_Set_Flag(UART_Index_t uart_index, uint16_t flag)
{
		if (uart_index < UART_MAX)
		{
				__disable_irq();
				uart_flag[uart_index] |= flag;
				__enable_irq();
		}
}

// race condition
static void UART_Clear_Flag(UART_Index_t uart_index, uint16_t flag)
{
		if (uart_index < UART_MAX)
		{
				__disable_irq();
				uart_flag[uart_index] &= ~flag;
				__enable_irq();
		}
}

static _Bool UART_Get_Flag(UART_Index_t uart_index, uint16_t flag)
{
    if (uart_index < UART_MAX)
    {
        return ((uart_flag[uart_index] & flag) == flag);
    }
    return 0;
}

// IRQ
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        UART_Set_Flag(UART1_INDEX, USART_FLAG_RXNE);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

void USART2_IRQHandler(void)
{
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        UART_Set_Flag(UART2_INDEX, USART_FLAG_RXNE);
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        UART_Set_Flag(UART3_INDEX, USART_FLAG_RXNE);
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}

const USART_InitTypeDef UARTx_Config[UART_MAX] = 
{
    [UART1_INDEX] = 
		{
        .USART_BaudRate = 9600,
        .USART_Mode = USART_Mode_Tx | USART_Mode_Rx,
        .USART_Parity = USART_Parity_No,
        .USART_StopBits = USART_StopBits_1,
        .USART_WordLength = USART_WordLength_8b,
        .USART_HardwareFlowControl = USART_HardwareFlowControl_None
    },
    [UART2_INDEX] = 
		{
        .USART_BaudRate = 9600,
        .USART_Mode = USART_Mode_Tx | USART_Mode_Rx,
        .USART_Parity = USART_Parity_No,
        .USART_StopBits = USART_StopBits_1,
        .USART_WordLength = USART_WordLength_8b,
        .USART_HardwareFlowControl = USART_HardwareFlowControl_None
    },
    [UART3_INDEX] = 
		{
        .USART_BaudRate = 9600,
        .USART_Mode = USART_Mode_Tx | USART_Mode_Rx,
        .USART_Parity = USART_Parity_No,
        .USART_StopBits = USART_StopBits_1,
        .USART_WordLength = USART_WordLength_8b,
        .USART_HardwareFlowControl = USART_HardwareFlowControl_None
    }
};

const UART_Driver_t UART_Driver = 
{
    .Pins_Init    = UART_Pins_Init,
    .Init         = UARTx_Init,
    .Enable_IRQ   = UART_Enable_IRQ,
	
    .Send_Char    = UART_Send_Char,
    .Send_String  = UART_Send_String,
    .Send_Line    = UART_Send_Line,
		.Receive_Char = UART_Receive_Char,
	
		.Set_Flag     = UART_Set_Flag,
		.Clear_Flag   = UART_Clear_Flag,
		.Get_Flag     = UART_Get_Flag
};
