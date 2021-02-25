#ifndef __PWM_H
#define __PWM_H

#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_rcc.h>










static __INLINE void changePWM_pulse(uint16_t pulse)
{
    TIM2->CCR3 = pulse;
}

void PWM_Init(void);


#endif /*__PWM_H */