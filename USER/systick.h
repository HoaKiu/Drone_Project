#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#include "stm32f10x.h"                  // Device heade

#ifdef __cplusplus
extern "C"{
#endif

#define NULL 0

typedef struct 
{
    void (*Init)(void);
	
    uint32_t (*Get_Tick)(void);              
    void (*Delay_Ms)(uint32_t time_ms);   
} 
SysTick_Driver_t;

extern const SysTick_Driver_t SysTick_Driver;

#ifdef __cplusplus
}
#endif

#endif /* __SYSTICK_H__ */
