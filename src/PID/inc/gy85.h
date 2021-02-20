/***********************************************************************
* FILENAME :        gy85.h
*
* DESCRIPTION :
*       9-DoF GY85 Lib for stm32
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
*       4Feb21     Lizhou        First Release Version.
*
*************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GY85_LIB_H
#define __GY85_LIB_H

#include <stm32f10x.h>
#include <HAL_like_i2c.h>       
#define GY85_ACCEL_I2CADDR                  0x53        /*!< ADXL345 - Three axis accelerometer               */
#define GY85_COMPASS_I2CADDR                0x1E        /*!< HMC5883L - Three axis magnetometer               */
#define GY85_GYRO_I2CADDR                   0x68        /*!< ITG3205 - Three axis gyroscope                   */

#define GY85_COMPASS_CRAADDR                0x00        /*!< magnetometer CRA Register Address                */
#define GY85_COMPASS_CRBADDR                0x01        /*!< magnetometer CRB Register Address                */
#define GY85_COMPASS_MODEADDR               0x02        /*!< magnetometer Mode Register Address               */
#define GY85_COMPASS_DATAXMSB               0x03        /*!< magnetometer DataX MSB Register Address          */
#define GY85_COMPASS_DATAXLSB               0x04        /*!< magnetometer DataX LSB Register Address          */
#define GY85_COMPASS_DATAZMSB               0x05        /*!< magnetometer DataZ MSB Register Address          */
#define GY85_COMPASS_DATAZLSB               0x06        /*!< magnetometer DataZ LSB Register Address          */
#define GY85_COMPASS_DATAYMSB               0x07        /*!< magnetometer DataY MSB Register Address          */
#define GY85_COMPASS_DATAYLSB               0x08        /*!< magnetometer DataY LSB Register Address          */

#define GY85_ACCEL_PWRADDR                  0x2D        /*!< accelerometer POWER_CTL Register Address         */
#define GY85_ACCEL_DATAX0                   0x32        /*!< accelerometer DataX LSB Register Address         */
#define GY85_ACCEL_DATAX1                   0x33        /*!< accelerometer DataX MSB Register Address         */
#define GY85_ACCEL_DATAY0                   0x34        /*!< accelerometer DataY LSB Register Address         */
#define GY85_ACCEL_DATAY1                   0x35        /*!< accelerometer DataY MSB Register Address         */
#define GY85_ACCEL_DATAZ0                   0x36        /*!< accelerometer DataZ LSB Register Address         */
#define GY85_ACCEL_DATAZ1                   0x37        /*!< accelerometer DataZ MSB Register Address         */
#define GY85_ACCEL_OFFSETX                  0x1E        /*!< accelerometer Offset X Register Address          */
#define GY85_ACCEL_OFFSETY                  0x1F        /*!< accelerometer Offset Y Register Address          */
#define GY85_ACCEL_OFFSETZ                  0x20        /*!< accelerometer Offset Z Register Address          */

#define GY85_GYRO_SMPLRTDIV                 0x15        /*!< gyroscope Sample Rate Divider Address             */
#define GY85_GYRO_DLPFFS                    0x16        /*!< gyroscope DLPF, Full Scale Address                */
#define GY85_GYRO_XOUTH                     0x1D        /*!< gyroscope DataX MSB Register Address              */
#define GY85_GYRO_XOUTL                     0x1E        /*!< gyroscope DataX LSB Register Address              */
#define GY85_GYRO_YOUTH                     0x1F        /*!< gyroscope DataY MSB Register Address              */
#define GY85_GYRO_YOUTL                     0x20        /*!< gyroscope DataY LSB Register Address              */
#define GY85_GYRO_ZOUTH                     0x21        /*!< gyroscope DataZ MSB Register Address              */
#define GY85_GYRO_ZOUTL                     0x22        /*!< gyroscope DataZ LSB Register Address              */
#define GY85_GYRO_PWR_MGM                   0x3E        /*!< gyroscope Power Management Register Address       */

#define GY85_ACCEL_CALI_COUNT               50

typedef struct
{
    int16_t                     input_0[3];
    int16_t                     input_1[3];
    int16_t                     input_2[3];
    int16_t                     output_0[3];
    int16_t                     output_1[3];
    int16_t                     output_2[3];
}gy85_gyro, GY85_GYRO[1];

typedef struct
{
    uint16_t                   updatecount;
    int16_t                    compass[3];
    int16_t                    accel[3];
    gy85_gyro                  gyro[3];
    int16_t                    gyro_offset[3];
}gy85_data, GY85_DATA[1];

typedef struct
{
    uint8_t             caliX[1];
    uint8_t             caliY[1];
    uint8_t             caliZ[1];
}accel_cali, ACCEL_CALI[1];

int8_t gy85_Init(gy85_data *gy85, i2cHandler *i2c);
int8_t gy85_updateData(i2cHandler *i2c, gy85_data *pData);
int8_t gy85_caliaccel(i2cHandler *i2c, accel_cali *cali);
int8_t gy85_caligyro(i2cHandler *i2c, gy85_data *pData);
#endif /*__GY85_LIB_H */