#include "delay.h"

static __IO uint32_t usTicks;

// void SysTick_Handler(){
//     if(usTicks != 0){
//         usTicks--;
//     }
// }

void DelayInit(){
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000000);
}

void DelayUs(uint32_t us)
{
    // Reload us value
    usTicks = us;
    // Wait until usTick reach zero
    while (usTicks);
}

void DelayMs(uint32_t ms)
{
    // Wait until ms reach zero
    while (ms--)
    {
        // Delay 1ms
        DelayUs(1000);
    }
}