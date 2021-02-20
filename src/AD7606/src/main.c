
/***********************************************************************
* FILENAME :        main.c
*
* DESCRIPTION :
*       AD7606 Lib for STM32 StdPeriph_Lib example.
*
* COPYRIGHT :
*       Copyright Lizhou 2021 All rights reserved.
*
* ORIGINAL AUTHOR(s) :    lisie31s@gmail.com        
*
* CREATE DATE :    26 Jan 2021
*
* CHANGES :
*       DATE        WHO           DETAIL
*       26Jan21     Lizhou        First Release Version.
*
*************************************************************************/
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>
#include <ad7606.h>
#include <delay.h>
#include <stdio.h>

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

int main(void) {

  DelayInit();
  USART1_Init();
  ad7606_Init();

  AD7606 ad7606data;
  ad7606_Reset();

  while(1){
    ad7606_FetchData(ad7606data);
    char data[100] = {0};
    sprintf(data, "%d %d %d %d %d %d %d %d \r\n", 
                  ad7606data->parsedData[0],ad7606data->parsedData[1],ad7606data->parsedData[2],ad7606data->parsedData[3],
                  ad7606data->parsedData[4],ad7606data->parsedData[5],ad7606data->parsedData[6],ad7606data->parsedData[7]);
    UART_Transmit((uint8_t *)data, ARRAYSIZE(data));
    DelayMs(1000);
  }
}


