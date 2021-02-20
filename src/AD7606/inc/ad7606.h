/***********************************************************************
* FILENAME :        ad7606.h
*
* DESCRIPTION :
*       AD7606 Lib for STM32 StdPeriph_Lib.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AD7606_Lib_H
#define __AD7606_Lib_H

#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>
#include <delay.h>

/** \defgroup  Need_sclae if need output real volt data
  * @{
  */
// #define NEEDSCALEDATA
/**
  * @}
  */

/** \defgroup   AD7606_SPI
  * @{
  */
#define AD7606_SPI          SPI1
/**
  * @}
  */

/** \defgroup   AD7606_SPI1_pin
  * @{
  */
#define SPI1_RCC            RCC_APB2Periph_SPI1
#define SPI1_GPIO_RCC       RCC_APB2Periph_GPIOA
#define SPI1_GPIO           GPIOA
#define SPI1_PIN_MOSI       GPIO_Pin_7
#define SPI1_PIN_MISO       GPIO_Pin_6
#define SPI1_PIN_SCK        GPIO_Pin_5
#define SPI1_PIN_SS         GPIO_Pin_4
/**
  * @}
  */

/** \defgroup   AD7606_peripheral_gpio_pin
  * @{
  */
#define AD7606_GPIO_RCC     RCC_APB2Periph_GPIOA
#define AD7606_GPIO         GPIOA
#define AD7606_BUSY         GPIO_Pin_1
#define AD7606_RESET        GPIO_Pin_2
#define AD7606_STARTCONV    GPIO_Pin_3
/**
  * @}
  */

/** \defgroup   data_count
  * @{
  */
#define TOTAL_RAW_BYTE          16
#define TOTAL_CHANNEL_COUNT     8
#define SCALE_FACTOR            0.00030517578125
/**
  * @}
  */

/** \defgroup   Slave_Select
  * @{
  */
#define ENABLE_SLAVE()      do{SPI1_GPIO->BRR = SPI1_PIN_SS;}while(0);     //Set Slave SS pin low
#define DISABLE_SLAVE()     do{SPI1_GPIO->BSRR = SPI1_PIN_SS;}while(0);    //Set Slave SS pin high
/**
  * @}
  */

/** \defgroup   Delay
  * @{
  */
#define DELAY_MS(ms)     DelayMs(ms)
#define DELAY_US(us)     DelayUs(us)
/**
  * @}
  */

/** 
  * @brief  ad7606_data_struct
  */
typedef struct {
    uint16_t rawData[TOTAL_RAW_BYTE];               /*!< Raw data from SPI */

    int16_t parsedData[TOTAL_CHANNEL_COUNT];        /*!< Parsed data from raw */

#ifdef NEEDSCALEDATA
    double scaleData[TOTAL_CHANNEL_COUNT];          /*!< times Scaler Factor */
#endif

}ad7606, AD7606[1];

/** @defgroup AD7606_Exported_Functions
  * @{
  */
void ad7606_Init(void);
void ad7606_Reset(void);
void parseRawBytes(int16_t *pData, uint16_t *raw);
void ad7606_FetchData(ad7606 *ad7606data);
/**
  * @}
  */

#endif /*__AD7606_Lib_H */