/***********************************************************************
* FILENAME :        ad7606.c
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
#include <ad7606.h>


/**
  * @brief  Initialize ad7606 periphery.(SPI, GPIO)
  * @param  None
  * @retval None
  */
void ad7606_Init(void)
{
    // Initialization struct
    SPI_InitTypeDef     SPI_InitStruct;
    GPIO_InitTypeDef    GPIO_InitStruct;

    // Initialize SPI
    RCC_APB2PeriphClockCmd(SPI1_RCC, ENABLE);
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_Init(AD7606_SPI, &SPI_InitStruct);
    SPI_Cmd(AD7606_SPI, ENABLE);

    // Initialize GPIOs
    // GPIOs of SPI
    RCC_APB2PeriphClockCmd(SPI1_GPIO_RCC, ENABLE);
    // MOSI, MISO, SCK
    GPIO_InitStruct.GPIO_Pin = SPI1_PIN_MOSI | SPI1_PIN_MISO | SPI1_PIN_SCK;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI1_GPIO, &GPIO_InitStruct);
    // SS
    GPIO_InitStruct.GPIO_Pin = SPI1_PIN_SS;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(SPI1_GPIO, &GPIO_InitStruct);
    // GPIOs of Other Periphery
    // BUSY
    GPIO_InitStruct.GPIO_Pin = AD7606_BUSY;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(AD7606_GPIO, &GPIO_InitStruct);
    // Reset, Start_Conversion
    GPIO_InitStruct.GPIO_Pin = AD7606_STARTCONV | AD7606_RESET;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(AD7606_GPIO, &GPIO_InitStruct);

    // Disable SPI slave device
    DISABLE_SLAVE();
}

/**
  * @brief  Reset AD7606, Please call it after ad7606_Init().
  * @param  None
  * @retval None
  */
void ad7606_Reset(void)
{
    GPIO_SetBits(AD7606_GPIO, AD7606_RESET);
    DELAY_MS(10);
    GPIO_ResetBits(AD7606_GPIO, AD7606_RESET);
    GPIO_SetBits(AD7606_GPIO, AD7606_STARTCONV);
}

/**
  * @brief  parse raw byte
  * @param  pData: Data parsed form raw data
  * @param  raw: raw data 
  * @retval None
  */
void parseRawBytes(int16_t *pData, uint16_t *raw) 
{
  pData[0] = (int16_t)((raw[0]  << 8) | raw[1]);
  pData[1] = (int16_t)((raw[2]  << 8) | raw[3]);
  pData[2] = (int16_t)((raw[4]  << 8) | raw[5]);
  pData[3] = (int16_t)((raw[6]  << 8) | raw[7]);
  pData[4] = (int16_t)((raw[8]  << 8) | raw[9]);
  pData[5] = (int16_t)((raw[10] << 8) | raw[11]);
  pData[6] = (int16_t)((raw[12] << 8) | raw[13]);
  pData[7] = (int16_t)((raw[14] << 8) | raw[15]);
}

/**
  * @brief  Get AD7606 Data. (Blocking)
  * @param  ad7606data: Data parsed form AD7606
  * @retval None
  */
void ad7606_FetchData(ad7606 *ad7606data)
{
    GPIO_ResetBits(AD7606_GPIO, AD7606_STARTCONV);
    DELAY_US(10);
    GPIO_SetBits(AD7606_GPIO, AD7606_STARTCONV);

    while(GPIO_ReadInputDataBit(AD7606_GPIO, AD7606_BUSY));

    ENABLE_SLAVE();
    for(uint8_t bytetoread = 0; bytetoread < TOTAL_RAW_BYTE;  bytetoread++)
    {
      while(SPI_I2S_GetFlagStatus(AD7606_SPI, SPI_I2S_FLAG_TXE) == RESET);
      SPI_I2S_SendData(AD7606_SPI, 0);
      while(SPI_I2S_GetFlagStatus(AD7606_SPI, SPI_I2S_FLAG_RXNE) == RESET);
      ad7606data->rawData[bytetoread] = SPI_I2S_ReceiveData(AD7606_SPI);
    }
    DISABLE_SLAVE();

    parseRawBytes(ad7606data->parsedData, ad7606data->rawData);

#ifdef NEEDSCALEDATA
    for(uint8_t channel = 0; channel < TOTAL_CHANNEL_COUNT; channel++){
        ad7606data->scaleData[channel] = ad7606data->parsedData[channel]*SCALE_FACTOR;
    }
#endif
}