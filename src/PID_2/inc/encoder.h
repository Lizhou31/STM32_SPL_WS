#ifndef __ENCODER_H
#define __ENCODER_H

#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_rcc.h>
#include <misc.h>

#define TIM3CH1_PIN GPIO_Pin_6
#define TIM3CH2_PIN GPIO_Pin_7
#define ENCODERTIM TIM3
#define TIM3_PERIOD 360*4-1

/* TODO : need counter side? */
typedef struct
{
    int32_t circle;
    int32_t rpm;
}m_encoder, M_ENCODER[1];

int8_t motor_encoder_init(m_encoder * mencoder);
int8_t update_motordata(m_encoder *mencoder);

#endif /*__ENCODER_H */