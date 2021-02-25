#ifndef __PWM_H
#define __PWM_H

#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_rcc.h>
#include <dsp_math_type.h>

#define MAX_DURATION 720U

static __INLINE void changePWM_pulse(q31_t norm_value)
{
    TIM2->CCR3 = map_value_u16(norm_value, MAX_DURATION) - 1;
}

void PWM_Init(void);

#endif /*__PWM_H */