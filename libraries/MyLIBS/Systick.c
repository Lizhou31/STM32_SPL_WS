#include <Systick.h>

static __IO uint64_t usTicks;

void SysTickInit(){
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000000);
}

void IncTicks(void){
    usTicks ++;
}
uint64_t GetTicks(void){
    return usTicks;
}
