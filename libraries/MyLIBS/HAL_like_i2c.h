/***********************************************************************
* FILENAME :        HAL_like_i2c.h
*
* DESCRIPTION :
*       Reference from STM32 HAL Lib
*
* COPYRIGHT :
*       Copyright Lizhou 2021 All rights reserved.
*
* ORIGINAL AUTHOR(s) :    lisie31s@gmail.com        
*
* CREATE DATE :    4 Feb 2021
*
* CHANGES :
*       DATE        WHO           DETAIL
*       3Feb21     Lizhou        First Release Version.
*
*************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HAL_LIKE_I2C_H
#define __HAL_LIKE_I2C_H

#include <stm32f10x.h>
#include <Systick.h>
/** @defgroup HAL_state_structure_definition HAL state structure definition
  * @brief  HAL State structure definition
  * @note  HAL I2C State value coding follow below described bitmap :
  *          b7-b6  Error information
  *             00 : No Error
  *             01 : Abort (Abort user request on going)
  *             10 : Timeout
  *             11 : Error
  *          b5     Peripheral initilisation status
  *             0  : Reset (Peripheral not initialized)
  *             1  : Init done (Peripheral initialized and ready to use. HAL I2C Init function called)
  *          b4     (not used)
  *             x  : Should be set to 0
  *          b3
  *             0  : Ready or Busy (No Listen mode ongoing)
  *             1  : Listen (Peripheral in Address Listen Mode)
  *          b2     Intrinsic process state
  *             0  : Ready
  *             1  : Busy (Peripheral busy with some configuration or internal operations)
  *          b1     Rx state
  *             0  : Ready (no Rx operation ongoing)
  *             1  : Busy (Rx operation ongoing)
  *          b0     Tx state
  *             0  : Ready (no Tx operation ongoing)
  *             1  : Busy (Tx operation ongoing)
  * @{
  */
typedef enum
{
  HAL_I2C_STATE_RESET             = 0x00U,   /*!< Peripheral is not yet Initialized         */
  HAL_I2C_STATE_READY             = 0x20U,   /*!< Peripheral Initialized and ready for use  */
  HAL_I2C_STATE_BUSY              = 0x24U,   /*!< An internal process is ongoing            */
  HAL_I2C_STATE_BUSY_TX           = 0x21U,   /*!< Data Transmission process is ongoing      */
  HAL_I2C_STATE_BUSY_RX           = 0x22U,   /*!< Data Reception process is ongoing         */
  HAL_I2C_STATE_LISTEN            = 0x28U,   /*!< Address Listen Mode is ongoing            */
  HAL_I2C_STATE_BUSY_TX_LISTEN    = 0x29U,   /*!< Address Listen Mode and Data Transmission
                                                 process is ongoing                         */
  HAL_I2C_STATE_BUSY_RX_LISTEN    = 0x2AU,   /*!< Address Listen Mode and Data Reception
                                                 process is ongoing                         */
  HAL_I2C_STATE_ABORT             = 0x60U,   /*!< Abort user request ongoing                */
  HAL_I2C_STATE_TIMEOUT           = 0xA0U,   /*!< Timeout state                             */
  HAL_I2C_STATE_ERROR             = 0xE0U    /*!< Error                                     */

} HAL_I2C_StateTypeDef;
/**
  * @}
  */

/** @defgroup HAL_mode_structure_definition HAL mode structure definition
  * @brief  HAL Mode structure definition
  * @note  HAL I2C Mode value coding follow below described bitmap :\n
  *          b7     (not used)\n
  *             x  : Should be set to 0\n
  *          b6\n
  *             0  : None\n
  *             1  : Memory (HAL I2C communication is in Memory Mode)\n
  *          b5\n
  *             0  : None\n
  *             1  : Slave (HAL I2C communication is in Slave Mode)\n
  *          b4\n
  *             0  : None\n
  *             1  : Master (HAL I2C communication is in Master Mode)\n
  *          b3-b2-b1-b0  (not used)\n
  *             xxxx : Should be set to 0000
  * @{
  */
typedef enum
{
  HAL_I2C_MODE_NONE               = 0x00U,   /*!< No I2C communication on going             */
  HAL_I2C_MODE_MASTER             = 0x10U,   /*!< I2C communication is in Master Mode       */
  HAL_I2C_MODE_SLAVE              = 0x20U,   /*!< I2C communication is in Slave Mode        */
  HAL_I2C_MODE_MEM                = 0x40U    /*!< I2C communication is in Memory Mode       */

} HAL_I2C_ModeTypeDef;
/**
  * @}
  */

/** @defgroup I2C_Error_Code_definition I2C Error Code definition
  * @brief  I2C Error Code definition
  * @{
  */
#define HAL_I2C_ERROR_NONE              0x00000000U    /*!< No error              */
#define HAL_I2C_ERROR_BERR              0x00000001U    /*!< BERR error            */
#define HAL_I2C_ERROR_ARLO              0x00000002U    /*!< ARLO error            */
#define HAL_I2C_ERROR_AF                0x00000004U    /*!< AF error              */
#define HAL_I2C_ERROR_OVR               0x00000008U    /*!< OVR error             */
#define HAL_I2C_ERROR_DMA               0x00000010U    /*!< DMA transfer error    */
#define HAL_I2C_ERROR_TIMEOUT           0x00000020U    /*!< Timeout Error         */
#define HAL_I2C_ERROR_SIZE              0x00000040U    /*!< Size Management error */
#define HAL_I2C_ERROR_DMA_PARAM         0x00000080U    /*!< DMA Parameter Error   */
#define HAL_I2C_WRONG_START             0x00000200U    /*!< Wrong start Error     */
#define HAL_I2C_ERROR_DISABLE           0x00000400U    /*!< I2C Disable Error     */
/**
  * @}
  */

/** 
  * @brief  i2cHandler_struct
  */
typedef struct 
{
    I2C_TypeDef                     *Instance;
    __IO HAL_I2C_StateTypeDef       State;
    __IO uint16_t                   size;
    __IO HAL_I2C_ModeTypeDef        Mode;
    __IO uint32_t                   ErrorCode;
} i2cHandler, I2CHANDLER[1];

/** @defgroup HAL_Like_I2C_Exported_Functions
  * @{
  */
void i2c_handler_Init(i2cHandler *i2c, I2C_TypeDef *I2C, HAL_I2C_ModeTypeDef Mode);
int8_t Mem_Write(i2cHandler *i2c, uint8_t DevAddress, uint8_t MemAddress, uint8_t *pData, uint16_t Size, uint64_t Timeout);
int8_t Mem_Read_8bits(i2cHandler *i2c, uint8_t DevAddress, uint8_t MemAddress, uint8_t *pData, uint16_t Size, uint64_t Timeout);
/**
  * @}
  */
#endif /*__HAL_LIKE_I2C_H */