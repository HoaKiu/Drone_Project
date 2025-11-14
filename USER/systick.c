#include "systick.h"

static volatile uint32_t tick_ms = 0;

// prototype
static void SysTick_Init(void);

static uint32_t Get_Tick(void);
static void Delay_Ms(uint32_t time_ms);

static void SysTick_Init(void)
{
		SystemCoreClockUpdate();
    if (SysTick_Config(SystemCoreClock / 1000))
    {
			// Loi: neu truyen gia tri > 24 bit
        while (1);
    }
}

static uint32_t Get_Tick(void)
{
		return tick_ms;
}

static void Delay_Ms(uint32_t time_ms)
{
		uint32_t current_tick = Get_Tick();
		while (Get_Tick() - current_tick < time_ms);
}

void SysTick_Handler(void)
{
		tick_ms++;
}

const SysTick_Driver_t SysTick_Driver = 
{
    .Init     = SysTick_Init,
	
    .Get_Tick = Get_Tick,
    .Delay_Ms = Delay_Ms
};
