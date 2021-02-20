#include <gy85.h>

int8_t gy85_Init(gy85_data *gy85, i2cHandler *i2c)
{

    int8_t err = 0;
    uint8_t cmd = 0x78;     // (8-average, 75 Hz, normal measurement)
    err |= Mem_Write(i2c, GY85_COMPASS_I2CADDR, GY85_COMPASS_CRAADDR, &cmd, sizeof(cmd), 0xFFFF);
    cmd = 0xA0;             // (Gain=5)
    err |= Mem_Write(i2c, GY85_COMPASS_I2CADDR, GY85_COMPASS_CRBADDR, &cmd, sizeof(cmd), 0xFFFF);
    cmd = 0x00;             // (Continuous-measurement mode)
    err |= Mem_Write(i2c, GY85_COMPASS_I2CADDR, GY85_COMPASS_MODEADDR, &cmd, sizeof(cmd), 0xFFFF);

    cmd = 0x08;
    err |= Mem_Write(i2c, GY85_ACCEL_I2CADDR, GY85_ACCEL_PWRADDR, &cmd, sizeof(cmd), 0xFFFF);

    cmd = 0x09;             // devide 10
    err |= Mem_Write(i2c, GY85_GYRO_I2CADDR, GY85_GYRO_SMPLRTDIV, &cmd, sizeof(cmd), 0xFFFF);
    cmd = 0x1E;             // ±2000°/sec, lp 188Hz, internal sample rate 1kHz
    err |= Mem_Write(i2c, GY85_GYRO_I2CADDR, GY85_GYRO_DLPFFS, &cmd, sizeof(cmd), 0xFFFF);
    cmd = 0x01;             // PLL with gyroX
    err |= Mem_Write(i2c, GY85_GYRO_I2CADDR, GY85_GYRO_PWR_MGM, &cmd, sizeof(cmd), 0xFFFF);

    gy85->updatecount = 0;

    return err;
}

static int8_t gy85_compassFetchData(i2cHandler *i2c, int16_t *pData)
{

    int8_t err = 0;
    uint8_t temp[6] = {0};
    err |= Mem_Read_8bits(i2c, GY85_COMPASS_I2CADDR, GY85_COMPASS_DATAXMSB, temp  , 1U, 0xFFFF);
    err |= Mem_Read_8bits(i2c, GY85_COMPASS_I2CADDR, GY85_COMPASS_DATAXLSB, temp+1, 1U, 0xFFFF);
    err |= Mem_Read_8bits(i2c, GY85_COMPASS_I2CADDR, GY85_COMPASS_DATAZMSB, temp+2, 1U, 0xFFFF);
    err |= Mem_Read_8bits(i2c, GY85_COMPASS_I2CADDR, GY85_COMPASS_DATAZLSB, temp+3, 1U, 0xFFFF);
    err |= Mem_Read_8bits(i2c, GY85_COMPASS_I2CADDR, GY85_COMPASS_DATAYMSB, temp+4, 1U, 0xFFFF);
    err |= Mem_Read_8bits(i2c, GY85_COMPASS_I2CADDR, GY85_COMPASS_DATAYLSB, temp+5, 1U, 0xFFFF);
    
    pData[0] = (int16_t)temp[1] | ((int16_t)(temp[0])<<8);
    pData[1] = (int16_t)temp[5] | ((int16_t)(temp[4])<<8);
    pData[2] = (int16_t)temp[3] | ((int16_t)(temp[2])<<8);
    return err;
}

static int8_t gy85_accelFetchData(i2cHandler *i2c, int16_t *pData)
{

    int8_t err = 0;
    uint8_t temp[6] = {0};
    err |= Mem_Read_8bits(i2c, GY85_ACCEL_I2CADDR, GY85_ACCEL_DATAX0, temp  , 1U, 0xFFFF);
    err |= Mem_Read_8bits(i2c, GY85_ACCEL_I2CADDR, GY85_ACCEL_DATAX1, temp+1, 1U, 0xFFFF);
    err |= Mem_Read_8bits(i2c, GY85_ACCEL_I2CADDR, GY85_ACCEL_DATAY0, temp+2, 1U, 0xFFFF);
    err |= Mem_Read_8bits(i2c, GY85_ACCEL_I2CADDR, GY85_ACCEL_DATAY1, temp+3, 1U, 0xFFFF);
    err |= Mem_Read_8bits(i2c, GY85_ACCEL_I2CADDR, GY85_ACCEL_DATAZ0, temp+4, 1U, 0xFFFF);
    err |= Mem_Read_8bits(i2c, GY85_ACCEL_I2CADDR, GY85_ACCEL_DATAZ1, temp+5, 1U, 0xFFFF);

    pData[0] = (int16_t)temp[0] | ((int16_t)(temp[1])<<8);
    pData[1] = (int16_t)temp[2] | ((int16_t)(temp[3])<<8);
    pData[2] = (int16_t)temp[4] | ((int16_t)(temp[5])<<8);

    return err;
}

static int8_t gy85_gyroFetchData(i2cHandler *i2c, gy85_gyro *pData)
{

    int8_t err = 0;
    uint8_t temp[6] = {0};
    err |= Mem_Read_8bits(i2c, GY85_GYRO_I2CADDR, GY85_GYRO_XOUTH, temp  , 1U, 0xFFFF);
    err |= Mem_Read_8bits(i2c, GY85_GYRO_I2CADDR, GY85_GYRO_XOUTL, temp+1, 1U, 0xFFFF);
    err |= Mem_Read_8bits(i2c, GY85_GYRO_I2CADDR, GY85_GYRO_YOUTH, temp+2, 1U, 0xFFFF);
    err |= Mem_Read_8bits(i2c, GY85_GYRO_I2CADDR, GY85_GYRO_YOUTL, temp+3, 1U, 0xFFFF);
    err |= Mem_Read_8bits(i2c, GY85_GYRO_I2CADDR, GY85_GYRO_ZOUTH, temp+4, 1U, 0xFFFF);
    err |= Mem_Read_8bits(i2c, GY85_GYRO_I2CADDR, GY85_GYRO_ZOUTL, temp+5, 1U, 0xFFFF);

    pData->input_0[0] = (int16_t)temp[1] | ((int16_t)(temp[0])<<8);
    pData->input_0[1] = (int16_t)temp[3] | ((int16_t)(temp[2])<<8);
    pData->input_0[2] = (int16_t)temp[5] | ((int16_t)(temp[4])<<8);

    return err;
}

int8_t gy85_caliaccel(i2cHandler *i2c, accel_cali *cali)
{
    uint16_t calicount = 0;
    uint8_t sample[3] = {0};
    int16_t samplecumulative[3] = {0};
    for(calicount = 0; calicount < GY85_ACCEL_CALI_COUNT; calicount++)
    {
        if(Mem_Read_8bits(i2c, GY85_ACCEL_I2CADDR, GY85_ACCEL_DATAX0, sample  , 1U, 0xFFFF)) return -1;
        if(Mem_Read_8bits(i2c, GY85_ACCEL_I2CADDR, GY85_ACCEL_DATAY0, sample+1, 1U, 0xFFFF)) return -1;
        if(Mem_Read_8bits(i2c, GY85_ACCEL_I2CADDR, GY85_ACCEL_DATAZ0, sample+2, 1U, 0xFFFF)) return -1;
        samplecumulative[0] += (int16_t)(int8_t)sample[0];
        samplecumulative[1] += (int16_t)(int8_t)sample[1];
        samplecumulative[2] += (int16_t)(int8_t)(sample[2]-256);
    }

    // some error, that's not round()
    cali->caliX[0] = (uint8_t)(samplecumulative[0]/50/4);
    cali->caliY[0] = (uint8_t)(samplecumulative[1]/50/4);
    cali->caliZ[0] = (uint8_t)(samplecumulative[2]/50/4);

    while(calicount < GY85_ACCEL_CALI_COUNT)
    {
        if(Mem_Read_8bits(i2c, GY85_ACCEL_I2CADDR, GY85_ACCEL_DATAX0, sample  , 1U, 0xFFFF)) return -1;
        if(Mem_Read_8bits(i2c, GY85_ACCEL_I2CADDR, GY85_ACCEL_DATAY0, sample+1, 1U, 0xFFFF)) return -1;
        if(Mem_Read_8bits(i2c, GY85_ACCEL_I2CADDR, GY85_ACCEL_DATAZ0, sample+2, 1U, 0xFFFF)) return -1;
        if(sample[0] < (cali->caliX[0] - 10) || sample[0] > (cali->caliX[1] + 10)) continue;
        else samplecumulative[0] += (int16_t)(int8_t)sample[0];
        if(sample[1] < (cali->caliY[0] - 10) || sample[1] > (cali->caliY[1] + 10)) continue;
        else samplecumulative[1] += (int16_t)(int8_t)sample[1];
        if((sample[2]-256) < (cali->caliZ[0] - 10) || (sample[2]-256) > (cali->caliZ[1] + 10)) continue;
        else samplecumulative[2] += (int16_t)(int8_t)(sample[2]-256);
    }

    // some error, that's not round()
    cali->caliX[0] = (uint8_t)-(samplecumulative[0]/50/4);
    cali->caliY[0] = (uint8_t)-(samplecumulative[1]/50/4);
    cali->caliZ[0] = (uint8_t)-(samplecumulative[2]/50/4);

    if(Mem_Write(i2c, GY85_ACCEL_I2CADDR, GY85_ACCEL_OFFSETX, (cali->caliX), 1U, 0xFFFF)) return -1;
    if(Mem_Write(i2c, GY85_ACCEL_I2CADDR, GY85_ACCEL_OFFSETY, (cali->caliY), 1U, 0xFFFF)) return -1;
    if(Mem_Write(i2c, GY85_ACCEL_I2CADDR, GY85_ACCEL_OFFSETZ, (cali->caliZ), 1U, 0xFFFF)) return -1;

    return 0;
}

int8_t gy85_caligyro(i2cHandler *i2c, gy85_data *pData)
{
    int8_t err = 0;
    int32_t accumulate_gyro[3] = {0};
    for(uint8_t count = 0; count < 10; count ++){
        err |= gy85_gyroFetchData(i2c, pData->gyro);
        accumulate_gyro[0] += pData->gyro->input_0[0];
        accumulate_gyro[1] += pData->gyro->input_0[1];
        accumulate_gyro[2] += pData->gyro->input_0[2];
    }
    
    pData->gyro_offset[0] = -(int16_t)(accumulate_gyro[0]/10);
    pData->gyro_offset[1] = -(int16_t)(accumulate_gyro[1]/10);
    pData->gyro_offset[2] = -(int16_t)(accumulate_gyro[2]/10);
    return err;
}

int8_t gy85_updateData(i2cHandler *i2c, gy85_data *pData)
{
    int8_t err = 0;
    if(pData->updatecount <= 0){
        err |= gy85_compassFetchData(i2c, pData->compass);
        err |= gy85_accelFetchData(i2c, pData->accel);
        pData->updatecount = (20);
    }
    pData->updatecount--;
    err |= gy85_gyroFetchData(i2c, pData->gyro);
    return err;
}