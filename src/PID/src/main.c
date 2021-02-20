#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_i2c.h>
#include <stm32f10x_usart.h>
#include <dsp_math_type.h>
#include <gy85.h>
#include <Systick.h>
#include <stdio.h>

#define I2Cx_RCC        RCC_APB1Periph_I2C1
#define I2Cx            I2C1
#define I2C_GPIO_RCC    RCC_APB2Periph_GPIOB
#define I2C_GPIO        GPIOB
#define I2C_PIN_SDA     GPIO_Pin_7
#define I2C_PIN_SCL     GPIO_Pin_6
#define ARRAYSIZE(arr) (sizeof(arr) / sizeof(arr[0]))

void SysTick_Handler(){
    IncTicks();
}

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

void i2c_init(i2cHandler *i2c)
{
    // Initialization struct
    I2C_InitTypeDef I2C_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;

    // Initialize I2C
    RCC_APB1PeriphClockCmd(I2Cx_RCC, ENABLE);
    I2C_InitStruct.I2C_ClockSpeed = 100000;
    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStruct.I2C_OwnAddress1 = 0x00;
    I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2Cx, &I2C_InitStruct);
    I2C_Cmd(I2Cx, ENABLE);
    i2c_handler_Init(i2c, I2Cx, HAL_I2C_MODE_NONE);

    // Initialize GPIO as open drain alternate function
    RCC_APB2PeriphClockCmd(I2C_GPIO_RCC, ENABLE);
    GPIO_InitStruct.GPIO_Pin = I2C_PIN_SCL | I2C_PIN_SDA;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C_GPIO, &GPIO_InitStruct);
}

int main()
{
    GY85_DATA gy85;
    I2CHANDLER i2c;
    ACCEL_CALI cali;
    __IO int8_t err = 0;
    uint64_t current = GetTicks();
    uint64_t time = 1000;
    SysTickInit();
    USART1_Init();
    i2c_init(i2c);
    err |= gy85_Init(gy85,i2c);
    while((GetTicks()-current) < time);
    gy85_caliaccel(i2c, cali);
    gy85_caligyro(i2c, gy85);
    while(1)
    {
      current = GetTicks();
      err = gy85_updateData(i2c,gy85);
      char data[60] = {'\0'};

      sprintf(data, "%d %d %d \r\n", gy85->gyro->input_0[0] + (gy85->gyro_offset[0]), gy85->gyro->input_0[1] + (gy85->gyro_offset[1]), gy85->gyro->input_0[2] + (gy85->gyro_offset[2]));
      // sprintf(data, "%d %d %d\r\n %d %d %d\r\n %d %d %d\r\n", gy85->compass[0], gy85->compass[1], gy85->compass[2]
      //                                          , gy85->accel[0], gy85->accel[1], gy85->accel[2]
      //                                          , gy85->gyro->input_0[0], gy85->gyro->input_0[1], gy85->gyro->input_0[2]);
                                              
      UART_Transmit((uint8_t *)data, ARRAYSIZE(data));
      while((GetTicks() - current) < time);
    }
}