/***********************************************************************
* FILENAME :        HAL_like_i2c.c
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

#include <HAL_like_i2c.h>

/**
  * @brief  Initialize i2c periphery.(SPI, GPIO)
  * @param  i2c: i2chandler struct (with some state define)
  * @param  I2C: SPL Default I2C struct (with registers)
  * @param  Mode: I2C Operation Mode (That is no effect now)
  * @retval None
  */
void i2c_handler_Init(i2cHandler *i2c, I2C_TypeDef *I2C, HAL_I2C_ModeTypeDef Mode)
{
    i2c->Instance = I2C;
    i2c->State = HAL_I2C_STATE_READY;
    i2c->Mode = Mode;
    i2c->ErrorCode = HAL_I2C_ERROR_NONE;
}

/**
 * @brief   Static function to Wait Busy Flag Reset until timeout
 * @param   i2c: i2cHandler struct
 * @param   tickstart: unsigned 64-bit integer tickstart
 * @param   Timeout: unsigned 64-bit integer Timeout
 * @retval  signed 8-bit int error (0: success -1: Failed)
 */
static int8_t I2C_WaitOnBUSYFlagUntilTimeout(i2cHandler *i2c, uint64_t tickstart, uint64_t Timeout)
{
    while((READ_BIT(i2c->Instance->SR2, I2C_SR2_BUSY) == (I2C_SR2_BUSY)) == SET)
    {
        /* Check for the Timeout */
        if(Timeout < MAX_DELAY)
        {
            if((GetTicks() - tickstart) > Timeout)
            {
                i2c->State      = HAL_I2C_STATE_READY;
                i2c->Mode       = HAL_I2C_MODE_NONE;
                i2c->ErrorCode  |= HAL_I2C_ERROR_TIMEOUT;
                return -1;
            }
        }
    }
    return 0;
}

/**
 * @brief   Static function to Wait SB Flag SET until timeout
 * @param   i2c: i2cHandler struct
 * @param   tickstart: unsigned 64-bit integer tickstart
 * @param   Timeout: unsigned 64-bit integer Timeout
 * @retval  signed 8-bit int error (0: success -1: Failed)
 */
static int8_t I2C_WaitOnSBFlagUntilTimeout(i2cHandler *i2c, uint64_t tickstart, uint64_t Timeout)
{
    while(((READ_BIT(i2c->Instance->SR1, I2C_SR1_SB) == (I2C_SR1_SB))) == RESET)
    {
        /* Check for the Timeout */
        if(Timeout < MAX_DELAY){
            if ((GetTicks() - tickstart) > Timeout)
            {
                i2c->State = HAL_I2C_STATE_READY;
                i2c->Mode = HAL_I2C_MODE_NONE;
                i2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
                if (READ_BIT(i2c->Instance->CR1, I2C_CR1_START) == I2C_CR1_START)
                    i2c->ErrorCode |= HAL_I2C_WRONG_START;
                return -1;
            }
        }
    }
    return 0;
}

/**
 * @brief   Static function to Wait ADDR Flag SET until timeout
 * @param   i2c: i2cHandler struct
 * @param   tickstart: unsigned 64-bit integer tickstart
 * @param   Timeout: unsigned 64-bit integer Timeout
 * @retval  signed 8-bit int error (0: success -1: Failed)
 */
static int8_t I2C_WaitOnADDRFlagUntilTimeout(i2cHandler *i2c, uint64_t tickstart, uint64_t Timeout)
{
    while ((READ_BIT(i2c->Instance->SR1, I2C_SR1_ADDR) == (I2C_SR1_ADDR)) == RESET)
    {
        if ((READ_BIT(i2c->Instance->SR1, I2C_SR1_AF) == (I2C_SR1_AF)))
        {
            /* Generate Stop */
            SET_BIT(i2c->Instance->CR1, I2C_CR1_STOP);

            /* Clear AF Flag */
            CLEAR_BIT(i2c->Instance->SR1, I2C_SR1_AF);

            i2c->State = HAL_I2C_STATE_READY;
            i2c->Mode = HAL_I2C_MODE_NONE;
            i2c->ErrorCode |= HAL_I2C_ERROR_AF;

            return -1;
        }

        /* Check for the Timeout */
        if (Timeout < MAX_DELAY)
        {
            if ((GetTicks() - tickstart) > Timeout)
            {
                i2c->State = HAL_I2C_STATE_READY;
                i2c->Mode = HAL_I2C_MODE_NONE;
                i2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;

                return -1;
            }
        }
    }
    return 0;
}

/**
 * @brief   Static function to Wait TXE Flag SET until timeout
 * @param   i2c: i2cHandler struct
 * @param   tickstart: unsigned 64-bit integer tickstart
 * @param   Timeout: unsigned 64-bit integer Timeout
 * @retval  signed 8-bit int error (0: success -1: Failed)
 */
static int8_t I2C_WaitOnTXEFlagUntilTimeout(i2cHandler *i2c, uint64_t tickstart, uint64_t Timeout)
{
    while ((READ_BIT(i2c->Instance->SR1, I2C_SR1_TXE) == (I2C_SR1_TXE)) == RESET)
    {
        /* Check if a NACK is detected */
        if ((READ_BIT(i2c->Instance->SR1, I2C_SR1_AF) == (I2C_SR1_AF)))
        {
            /* Generate Stop */
            SET_BIT(i2c->Instance->CR1, I2C_CR1_STOP);

            /* Clear AF Flag */
            CLEAR_BIT(i2c->Instance->SR1, I2C_SR1_AF);

            i2c->State = HAL_I2C_STATE_READY;
            i2c->Mode = HAL_I2C_MODE_NONE;
            i2c->ErrorCode |= HAL_I2C_ERROR_AF;

            return -1;
        }
        /* Check for the Timeout */
        if (Timeout < MAX_DELAY)
        {
            if ((GetTicks() - tickstart) > Timeout)
            {
                i2c->State = HAL_I2C_STATE_READY;
                i2c->Mode = HAL_I2C_MODE_NONE;
                i2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;

                return -1;
            }
        }
    }
    return 0;
}

/**
 * @brief   Static function to Wait BTF Flag SET until timeout
 * @param   i2c: i2cHandler struct
 * @param   tickstart: unsigned 64-bit integer tickstart
 * @param   Timeout: unsigned 64-bit integer Timeout
 * @retval  signed 8-bit int error (0: success -1: Failed)
 */
static int8_t I2C_WaitOnBTFFlagUntilTimeout(i2cHandler *i2c, uint64_t tickstart, uint64_t Timeout)
{
    while ((READ_BIT(i2c->Instance->SR1, I2C_SR1_BTF) == (I2C_SR1_BTF)) == RESET)
    {
        /* Check if a NACK is detected */
        if ((READ_BIT(i2c->Instance->SR1, I2C_SR1_AF) == (I2C_SR1_AF)))
        {
            /* Generate Stop */
            SET_BIT(i2c->Instance->CR1, I2C_CR1_STOP);

            /* Clear AF Flag */
            CLEAR_BIT(i2c->Instance->SR1, I2C_SR1_AF);

            i2c->State = HAL_I2C_STATE_READY;
            i2c->Mode = HAL_I2C_MODE_NONE;
            i2c->ErrorCode |= HAL_I2C_ERROR_AF;

            return -1;
        }
        /* Check for the Timeout */
        if (Timeout < MAX_DELAY)
        {
            if ((GetTicks() - tickstart) > Timeout)
            {
                i2c->State = HAL_I2C_STATE_READY;
                i2c->Mode = HAL_I2C_MODE_NONE;
                i2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;

                return -1;
            }
        }
    }
    return 0;
}

/**
 * @brief   Static function to Wait RXNE Flag SET until timeout
 * @param   i2c: i2cHandler struct
 * @param   tickstart: unsigned 64-bit integer tickstart
 * @param   Timeout: unsigned 64-bit integer Timeout
 * @retval  signed 8-bit int error (0: success -1: Failed)
 */
static int8_t I2C_WaitOnRXNEFlagUntilTimeout(i2cHandler *i2c, uint64_t tickstart, uint64_t Timeout)
{
    while((READ_BIT(i2c->Instance->SR1, I2C_SR1_RXNE) == (I2C_SR1_RXNE)) == RESET)
    {
        if((READ_BIT(i2c->Instance->SR1, I2C_SR1_STOPF) == (I2C_SR1_STOPF)))
        {
            /* Clear STOP Flag */
            /*learing this flag is done by a read access to the I2Cx_SR1 register followed by a write access to I2Cx_CR1 register. */
            __IO uint32_t tmpreg;
            tmpreg = i2c->Instance->SR1;
            (void) tmpreg;
            SET_BIT(i2c->Instance->CR1, I2C_CR1_PE);

            i2c->State               = HAL_I2C_STATE_READY;
            i2c->Mode                = HAL_I2C_MODE_NONE;
            i2c->ErrorCode           |= HAL_I2C_ERROR_NONE;

            return -1;
        }
        /* Check for the Timeout */
        if (Timeout < MAX_DELAY)
        {
            if ((GetTicks() - tickstart) > Timeout)
            {
                i2c->State = HAL_I2C_STATE_READY;
                i2c->Mode = HAL_I2C_MODE_NONE;
                i2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;

                return -1;
            }
        }
    }
    return 0;
}

/**
 * @brief   I2C Memory Write function (Only support 8-bit now)
 * @param   i2c: i2cHandler struct
 * @param   DevAddress: unsigned 8-bit Device Address
 * @param   MemAddress: unsigned 8-bit Register Address
 * @param   pData: unsigned 8-bit Data pointer
 * @param   size: unsigned 16-bit data size (now it only support 1U / 1-byte)
 * @param   Timeout: unsigned 64-bit integer Timeout
 * @retval  signed 8-bit int error (0: success -1: Failed)
 */
int8_t Mem_Write(i2cHandler *i2c, uint8_t DevAddress, uint8_t MemAddress, uint8_t *pData, uint16_t Size, uint64_t Timeout)
{
    __IO uint64_t tickstart = GetTicks();
    if(i2c->State == HAL_I2C_STATE_READY)
    {
        /* Wait until BUSY flag is reset */
        if(I2C_WaitOnBUSYFlagUntilTimeout(i2c, tickstart, Timeout)) return -1;

        /* Check if the I2C is already enabled */
        if((i2c->Instance->CR1 & I2C_CR1_PE) != I2C_CR1_PE)
        {
            i2c->State      = HAL_I2C_STATE_READY;
            i2c->Mode       = HAL_I2C_MODE_NONE;
            i2c->ErrorCode  |= HAL_I2C_ERROR_DISABLE;
            return -1;
        }

        /* Disable Pos */
        CLEAR_BIT(i2c->Instance->CR1, I2C_CR1_POS);

        i2c->State = HAL_I2C_STATE_BUSY_TX;
        i2c->Mode = HAL_I2C_MODE_MEM;
        i2c->ErrorCode = HAL_I2C_ERROR_NONE;

        /* Generate Start */
        SET_BIT(i2c->Instance->CR1, I2C_CR1_START);
        /* Wait until SB flag is set */
        if(I2C_WaitOnSBFlagUntilTimeout(i2c, tickstart, Timeout)) return -1;

        /* Send Slave ADDR address (W) */
        MODIFY_REG(i2c->Instance->DR, I2C_DR_DR, (DevAddress<<1) & (uint8_t)(~I2C_OAR1_ADD0));
        /* Wait until ADDR flag is set */
        if(I2C_WaitOnADDRFlagUntilTimeout(i2c, tickstart, Timeout)) return -1;
        /* Clear ADDR flag */
        /* Clearing this flag is done by a read access to the I2Cx_SR1 register followed by a read access to the I2Cx_SR2 register.*/
        __IO uint32_t tmpreg;
        tmpreg = i2c->Instance->SR1;
        (void) tmpreg;
        tmpreg = i2c->Instance->SR2;
        (void) tmpreg;

        /* Wait until TXE flag is set */
        if(I2C_WaitOnTXEFlagUntilTimeout(i2c, tickstart, Timeout)) return -1;

        /* Send 8-bit MemAddress */
        MODIFY_REG(i2c->Instance->DR, I2C_DR_DR, MemAddress);

        while(Size > 0U)
        {
            /* Wait until TXE flag is set */
            if(I2C_WaitOnTXEFlagUntilTimeout(i2c, tickstart, Timeout)) return -1;

            /* Send 8-bit Data */
            MODIFY_REG(i2c->Instance->DR, I2C_DR_DR, *pData);
            Size--;
            *pData++;
            /* Wait until BTF flag is set */
            if(I2C_WaitOnBTFFlagUntilTimeout(i2c, tickstart, Timeout)) return -1;
        }
        
        /* Generate Stop */
        SET_BIT(i2c->Instance->CR1, I2C_CR1_STOP);
        i2c->State = HAL_I2C_STATE_READY;
        i2c->Mode = HAL_I2C_MODE_NONE;

        return 0;
    }
    else
    {
        return -1;
    }
}

/**
 * @brief   I2C Memory Read function (Only support 8-bit now)
 * @param   i2c: i2cHandler struct
 * @param   DevAddress: unsigned 8-bit Device Address
 * @param   MemAddress: unsigned 8-bit Register Address
 * @param   pData: unsigned 8-bit Data pointer
 * @param   size: unsigned 16-bit data size (now it only support 1U / 1-byte)
 * @param   Timeout: unsigned 64-bit integer Timeout
 * @retval  signed 8-bit int error (0: success -1: Failed)
 */
int8_t Mem_Read_8bits(i2cHandler *i2c, uint8_t DevAddress, uint8_t MemAddress, uint8_t *pData, uint16_t Size, uint64_t Timeout)
{
    __IO uint64_t tickstart = GetTicks();
    if(i2c->State == HAL_I2C_STATE_READY)
    {
        /* Wait until BUSY flag is reset */
        if(I2C_WaitOnBUSYFlagUntilTimeout(i2c, tickstart, Timeout)) return -1;

        /* Check if the I2C is already enabled */
        if((i2c->Instance->CR1 & I2C_CR1_PE) != I2C_CR1_PE)
        {
            i2c->State      = HAL_I2C_STATE_READY;
            i2c->Mode       = HAL_I2C_MODE_NONE;
            i2c->ErrorCode  |= HAL_I2C_ERROR_DISABLE;
            return -1;
        }

        /* Disable Pos */
        CLEAR_BIT(i2c->Instance->CR1, I2C_CR1_POS);

        i2c->State = HAL_I2C_STATE_BUSY_TX;
        i2c->Mode = HAL_I2C_MODE_MEM;
        i2c->ErrorCode = HAL_I2C_ERROR_NONE;
        i2c->size = Size;

        /* Enable Acknowledge */
        SET_BIT(i2c->Instance->CR1, I2C_CR1_ACK);

        /* Generate Start */
        SET_BIT(i2c->Instance->CR1, I2C_CR1_START);
        /* Wait until SB flag is set */
        if(I2C_WaitOnSBFlagUntilTimeout(i2c, tickstart, Timeout)) return -1;

        /* Send Slave ADDR flag (W) */
        MODIFY_REG(i2c->Instance->DR, I2C_DR_DR, (DevAddress<<1) & (uint8_t)(~I2C_OAR1_ADD0));
        /* Wait until ADDR flag is set */
        if(I2C_WaitOnADDRFlagUntilTimeout(i2c, tickstart, Timeout)) return -1;
        /* Clear ADDR flag */
        /* Clearing this flag is done by a read access to the I2Cx_SR1 register followed by a read access to the I2Cx_SR2 register.*/
        __IO uint32_t tmpreg;
        tmpreg = i2c->Instance->SR1;
        (void) tmpreg;
        tmpreg = i2c->Instance->SR2;
        (void) tmpreg;

        /* Wait until TXE flag is set */
        if(I2C_WaitOnTXEFlagUntilTimeout(i2c, tickstart, Timeout)) return -1;

        /* Send 8-bit MemAddress */
        MODIFY_REG(i2c->Instance->DR, I2C_DR_DR, MemAddress);
        /* Wait until TXE flag is set */
        if(I2C_WaitOnTXEFlagUntilTimeout(i2c, tickstart, Timeout)) return -1;

        /* Generate ReStart */
        SET_BIT(i2c->Instance->CR1, I2C_CR1_START);
        /* Wait until SB flag is set */
        if(I2C_WaitOnSBFlagUntilTimeout(i2c, tickstart, Timeout)) return -1;

        /* Send Slave ADDR flag (R) */
        MODIFY_REG(i2c->Instance->DR, I2C_DR_DR, (DevAddress<<1) | (uint8_t)(I2C_OAR1_ADD0));
        /* Wait until ADDR flag is set */
        if(I2C_WaitOnADDRFlagUntilTimeout(i2c, tickstart, Timeout)) return -1;

        if(i2c->size == 1U){
            /* Disable Acknowledge */
            CLEAR_BIT(i2c->Instance->CR1, I2C_CR1_ACK);

            __disable_irq();

            /* Clear ADDR flag */
            /* Clearing this flag is done by a read access to the I2Cx_SR1 register followed by a read access to the I2Cx_SR2 register.*/
            __IO uint32_t tmpreg;
            tmpreg = i2c->Instance->SR1;
            (void) tmpreg;
            tmpreg = i2c->Instance->SR2;
            (void) tmpreg;
            /* Generate Stop */
            SET_BIT(i2c->Instance->CR1, I2C_CR1_STOP);

            __enable_irq();
        }
        while(Size > 0U){
            if (i2c->size == 1U)
            {
                /* Wait until RXNE flag is set */
                if(I2C_WaitOnRXNEFlagUntilTimeout(i2c, tickstart, Timeout)) return -1;
                
                /* Read data from DR */
                *pData = (uint8_t)(READ_BIT(i2c->Instance->DR, I2C_DR_DR));
            }
            Size--;
        }
        i2c->State = HAL_I2C_STATE_READY;
        i2c->Mode = HAL_I2C_MODE_NONE;
        return 0;
    }
    else
    {
        return -1;
    }
}
