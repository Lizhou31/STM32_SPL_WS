#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_tim.h>
#include <Systick.h>
#include <encoder.h>
#include <misc.h>
#define ARRAYSIZE(arr) (sizeof(arr) / sizeof(arr[0]))

void SysTick_Handler(){
    IncTicks();
}

void USART1_Init(void)
{
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
  /* Baud rate 115200, 8-bit data, One stop bit
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

void UART_Transmit(uint8_t *data, uint16_t size)
{
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
  for(uint16_t i = 0; i< size; i++){
    USART_SendData(USART1, *(data+i));
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
  }
}

int main()
{
    M_ENCODER mencoder;
    SysTickInit();
    motor_encoder_init(mencoder);
    USART1_Init();
    uint64_t lasttime = GetTicks();
    while(1)
    {
        while((GetTicks() - lasttime) < 100000);
        update_motordata_100ms(mencoder);
        lasttime = GetTicks();
        char data[10] = {'\0'};
        sprintf(data, "%d\r\n", mencoder->rpm);
        UART_Transmit((uint8_t *)data, ARRAYSIZE(data));
    }

}