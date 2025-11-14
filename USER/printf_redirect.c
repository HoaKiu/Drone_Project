#include "printf_redirect.h"

#pragma import(__use_no_semihosting)

struct __FILE 
{ 
		int handle; 
};
FILE __stdout;

static void USART1_Send_Char(char _chr) 
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, (uint16_t)_chr);
}

int fputc(int ch, FILE *f) 
{
    (void)f;
    USART1_Send_Char((char)ch);
    return ch;
}

void _sys_exit(int return_code) 
{
    (void)return_code;
    while (1);
}
