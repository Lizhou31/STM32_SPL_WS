#ifndef __ENCODER_H
#define __ENCODER_H

#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_rcc.h>
#include <dsp_math_type.h>
#include <misc.h>

#define TIM3CH1_PIN GPIO_Pin_6
#define TIM3CH2_PIN GPIO_Pin_7
#define ENCODERTIM TIM3
#define TIM3_PERIOD 2 * 4 - 1

#define MAX_RPM 60.0f

/* TODO : need counter side? */
typedef struct
{
    int32_t roataion_count;
    int32_t rpm;         //actually rotation per 6s
    q31_t normalize_rpm; //actually rotation per 6s
} m_encoder, M_ENCODER[1];

int8_t motor_encoder_init(m_encoder *mencoder);
int8_t update_motordata_100ms(m_encoder *mencoder);

#endif /*__ENCODER_H */