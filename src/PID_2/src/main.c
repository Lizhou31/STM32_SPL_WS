#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_tim.h>
#include <dsp_pid.h>
#include <Systick.h>
#include <encoder.h>
#include <pwm.h>
#include <misc.h>
#define ARRAYSIZE(arr) (sizeof(arr) / sizeof(arr[0]))
M_ENCODER mencoder;
PID_INSTANCE_Q31 pid_instance;
float change;

void SysTick_Handler()
{
    IncTicks();
}

void USART1_Init(void)
{
    USART_InitTypeDef usart1_init_struct;
    GPIO_InitTypeDef gpioa_init_struct;

    /* Enalbe clock for USART1, AFIO and GPIOA */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO |
                               RCC_APB2Periph_GPIOA,
                           ENABLE);

    /* GPIOA PIN9 alternative function Tx */
    gpioa_init_struct.GPIO_Pin = GPIO_Pin_9;
    gpioa_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpioa_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpioa_init_struct);
    /* GPIOA PIN9 alternative function Rx */
    gpioa_init_struct.GPIO_Pin = GPIO_Pin_10;
    gpioa_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpioa_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpioa_init_struct);

    /* Enable USART1 */
    USART_Cmd(USART1, ENABLE);
    /* Baud rate 115200, 8-bit data, One stop bit
   * No parity, Do both Rx and Tx, No HW flow control
  */
    usart1_init_struct.USART_BaudRate = 115200;
    usart1_init_struct.USART_WordLength = USART_WordLength_8b;
    usart1_init_struct.USART_StopBits = USART_StopBits_1;
    usart1_init_struct.USART_Parity = USART_Parity_No;
    usart1_init_struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart1_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    /* Configure USART1 */
    USART_Init(USART1, &usart1_init_struct);
}

void UART_Transmit(uint8_t *data, uint16_t size)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        ;
    for (uint16_t i = 0; i < size; i++)
    {
        USART_SendData(USART1, *(data + i));
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
            ;
    }
}

void InitTIM4(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Init TIM3 CH1 and CH2 PIN for encoder */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
    TIM_TimeBaseInitStructure.TIM_Period = 3600 - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 1000;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);

    TIM_ClearFlag(TIM4, TIM_FLAG_Update);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_SetCounter(TIM4, 0);
    TIM_Cmd(TIM4, ENABLE);
}

void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4, TIM_FLAG_Update) == SET)
    {
        update_motordata_50ms(mencoder);
        q31_t output = pid_q31(pid_instance, float_to_q31(change)-mencoder->normalize_rpm);
        changePWM_pulse(output);
        // char data[10] = {'\0'};
        // sprintf(data, "%d\r\n",mencoder->rpm);
        // UART_Transmit((uint8_t *)data, ARRAYSIZE(data));
    }
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
}

int main()
{
    SysTickInit();
    motor_encoder_init(mencoder);
    PWM_Init();
    USART1_Init();    
    pid_instance->Kp = float_to_q31(0.3f);
    pid_instance->Ki = float_to_q31(0.3f);
    pid_instance->Kd = float_to_q31(0.2f);
    pid_init_q31(pid_instance, 1);
    InitTIM4();
    uint8_t i = 0,inverse = 1;
    uint64_t lasttime = GetTicks();
    while (1)
    {
        while ((GetTicks() - lasttime) < 100000)
            ;
        lasttime = GetTicks();
        i++;
        if(i>=100)
        {
            i=0;
            change = inverse?change+0.1f:change-0.1f;
            if(change>=1.f) inverse = 0;
            else if(change<=0.0f) inverse = 1;
        }
        char data[10] = {'\0'};
        sprintf(data, "%d\r\n",mencoder->rpm);
        UART_Transmit((uint8_t *)data, ARRAYSIZE(data));
    }
}