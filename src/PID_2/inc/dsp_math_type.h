/***********************************************************************
* FILENAME :        dsp_math_type.h
*
* DESCRIPTION :
*       Reference from CMSIS DSP Lib
*
* COPYRIGHT :
*       Copyright Lizhou 2021 All rights reserved.
*
* ORIGINAL AUTHOR(s) :    lisie31s@gmail.com        
*
* CREATE DATE :    3 Feb 2021
*
* CHANGES :
*       DATE        WHO           DETAIL
*       3Feb21     Lizhou        First Release Version.
*
*************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DSP_MATH_TYPE_LIB_H
#define __DSP_MATH_TYPE_LIB_H

#include <stm32f10x.h>
#include <core_cm3.h>

#if defined(__GNUC__)
#define CMSIS_INLINE __attribute__((always_inline))
#elif defined(__CC_ARM)
#define CMSIS_INLINE __attribute__((always_inline))
#endif

/**
 * @brief 32-bit fractional data type in 1.31 format
 */
typedef int32_t q31_t;

/**
 * @brief 64-bit fractional data type in 1.63 fromat
 */
typedef int64_t q63_t;

/**
 * @brief Clips Q63 to Q31 values.
 */
CMSIS_INLINE static __INLINE q31_t clip_q63_to_q31(q63_t x)
{
    return ((q31_t)(x >> 32) != ((q31_t)x >> 31)) ? ((0x7FFFFFFF) ^ ((q31_t)(x >> 63))) : (q31_t)x;
}

/**
 * @brief C custom defined q31_add
 */
CMSIS_INLINE static __INLINE q31_t add_q31(q31_t srcA, q31_t srcB)
{
    return clip_q63_to_q31((q63_t)srcA + (q31_t)srcB);
}

/**
 * @brief C custom defined q31_sub
 */
CMSIS_INLINE static __INLINE q31_t sub_q31(q31_t srcA, q31_t srcB)
{
    return clip_q63_to_q31((q63_t)srcA + (q31_t)srcB);
}

CMSIS_INLINE static __INLINE float q31_to_float(q31_t value)
{
    return (float)value / 2147483648.0f;
}

CMSIS_INLINE static __INLINE q31_t float_to_q31(float value)
{
    return clip_q63_to_q31((q63_t) (value * 2147483648.0f));
}

CMSIS_INLINE static __INLINE uint16_t map_value_u16(q31_t norm_value, uint16_t max_value)
{
    return (uint16_t)(q31_to_float(norm_value) * (float)max_value);
}
#endif /*__DSP_MATH_TYPE_LIB_H */
