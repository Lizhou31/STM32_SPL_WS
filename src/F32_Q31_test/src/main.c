#include <stdio.h>
#include <stm32f10x.h>
#include <Systick.h>

void SysTick_Handler()
{
    IncTicks();
}

int main(void)
{
    SysTickInit();
    __IO uint64_t current = GetTicks();
    while(1)
    {
        while(GetTicks() - current < 1000000);
        printf("Hello World!\n");
        current = GetTicks();
    }
    return 0;
}