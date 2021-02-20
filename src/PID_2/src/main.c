#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_tim.h>
#include <misc.h>
int circle_count = 0;
#define ARRAYSIZE(arr) (sizeof(arr) / sizeof(arr[0]))


void USART1_Init(void){
  USART_InitTypeDef usart1_init_struct;
  GPIO_InitTypeDef gpioa_init_struct;

  /* Enalbe clock for USART1, AFIO and GPIOA */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO | 
                         RCC_APB2Periph_GPIOA, ENABLE);
                            
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
  /* Baud rate 9600, 8-bit data, One stop bit
   * No parity, Do both Rx and Tx, No HW flow control
  */
  usart1_init_struct.USART_BaudRate = 115200;   
  usart1_init_struct.USART_WordLength = USART_WordLength_8b;  
  usart1_init_struct.USART_StopBits = USART_StopBits_1;   
  usart1_init_struct.USART_Parity = USART_Parity_No ;
  usart1_init_struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  usart1_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  /* Configure USART1 */
  USART_Init(USART1, &usart1_init_struct);
}

void UART_Transmit(uint8_t *data, uint16_t size){
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
  for(uint16_t i = 0; i< size; i++){
    USART_SendData(USART1, *(data+i));
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
  }
}

void TIM3_Int_Init()
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
    TIM_TimeBaseInitStructure.TIM_Period = 360*4-1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 0x0;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);

    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    TIM_ITConfig(TIM3, TIM_IT_Update,ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_SetCounter(TIM3,0);
    TIM_Cmd(TIM3, ENABLE);

}

void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3,TIM_FLAG_Update)==SET)
    {
        if((TIM3->CR1>>4 & 0x01) == 0)
            circle_count++;
        else if((TIM3->CR1>>4 & 0x01) == 1){
            circle_count--;
        }
    }
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
}

int main()
{
    TIM3_Int_Init();
    USART1_Init();

    while(1)
    {
        char data[10] = {'\0'};
        sprintf(data, "%d\r\n", circle_count);
        UART_Transmit((uint8_t *)data, ARRAYSIZE(data));
    }

}