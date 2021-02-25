#include <encoder.h>

static __IO int32_t circle_count;

int8_t motor_encoder_init(m_encoder *mencoder)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Init TIM3 CH1 and CH2 PIN for encoder */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = TIM3CH1_PIN | TIM3CH2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Init TIM3 for encoder */
    RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM3EN, ENABLE);
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
    TIM_TimeBaseInitStructure.TIM_Period = TIM3_PERIOD;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 0x0;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(ENCODERTIM, &TIM_TimeBaseInitStructure);
    TIM_EncoderInterfaceConfig(ENCODERTIM, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 0x4;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);

    TIM_ClearFlag(ENCODERTIM, TIM_FLAG_Update);
    TIM_ITConfig(ENCODERTIM, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_SetCounter(ENCODERTIM, 0);
    TIM_Cmd(ENCODERTIM, ENABLE);

    return 0;
}

void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_FLAG_Update) == SET)
    {
        if ((TIM3->CR1 >> 4 & 0x01) == 0)
            circle_count++;
        else if ((TIM3->CR1 >> 4 & 0x01) == 1)
        {
            circle_count--;
        }
    }
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}

int8_t update_motordata_100ms(m_encoder *mencoder)
{
    int32_t circle = circle_count;
    circle_count = 0;
    mencoder->roataion_count = circle;
    mencoder->rpm = (int32_t)(((float)circle / 3)); // circle_count*10*60/180
    mencoder->normalize_rpm = float_to_q31((float)mencoder->rpm / MAX_RPM);
    return 0;
}