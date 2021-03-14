/***********************************************************************
* FILENAME :        PID_Q31.h
*
* DESCRIPTION :
*       PID Lib Pack with Q31 format reference from CMSIS
*
* COPYRIGHT :
*       Copyright Lizhou 2021 All rights reserved.
*
* ORIGINAL AUTHOR(s) :    lisie31s@gmail.com        
*
* CREATE DATE :    14 Mar 2021
*
* CHANGES :
*       DATE            WHO           DETAIL
*       14Mar21     Lizhou        First Release Version.
*
*************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PID_Q31_H
#define __PID_Q31_H

#include "stm32f10x.h"
#include "core_cm3.h"
#include <string.h>

#if defined(__GNUC__)
#define CMSIS_INLINE __attribute__((always_inline))
#elif defined(__CC_ARM)
#define CMSIS_INLINE __attribute__((always_inline))
#endif

/**
 * @defgroup Q31 format util
 * @{
 */
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
    return clip_q63_to_q31((q63_t)(value * 2147483648.0f));
}

CMSIS_INLINE static __INLINE uint16_t map_value_u16(q31_t norm_value, uint16_t max_value)
{
    return (uint16_t)(q31_to_float(norm_value) * (float)max_value);
}
/**
  * @}
  */

/**
   * @brief Instance structure for the Q31 PID Control.
   */
typedef struct
{
    q31_t A0;       /**< The derived gain, A0 = Kp + Ki + Kd . */
    q31_t A1;       /**< The derived gain, A1 = -Kp - 2Kd. */
    q31_t A2;       /**< The derived gain, A2 = Kd . */
    q31_t state[3]; /**< The state array of length 3. */
    q31_t Kp;       /**< The proportional gain. */
    q31_t Ki;       /**< The integral gain. */
    q31_t Kd;       /**< The derivative gain. */

} pid_instance_q31, PID_INSTANCE_Q31[1];

/**
   * @brief  Initialization function for the Q31 PID Control.
   * @param[in,out] S               points to an instance of the Q15 PID structure.
   * @param[in]     resetStateFlag  flag to reset the state. 0 = no change in state 1 = reset the state.
   */
void pid_init_q31(
    pid_instance_q31 *S,
    int32_t resetStateFlag);

/**
   * @brief  Reset function for the Q31 PID Control.
   * @param[in,out] S   points to an instance of the Q31 PID Control structure
   */

void pid_reset_q31(
    pid_instance_q31 *S);

/**
  @brief         Process function for the Q31 PID Control.
  @param[in,out] S  points to an instance of the Q31 PID Control structure
  @param[in]     in  input sample to process
  @return        processed output sample.

  \par Scaling and Overflow Behavior
         The function is implemented using an internal 64-bit accumulator.
         The accumulator has a 2.62 format and maintains full precision of the intermediate multiplication results but provides only a single guard bit.
         Thus, if the accumulator result overflows it wraps around rather than clip.
         In order to avoid overflows completely the input signal must be scaled down by 2 bits as there are four additions.
         After all multiply-accumulates are performed, the 2.62 accumulator is truncated to 1.32 format and then saturated to 1.31 format.
 */
CMSIS_INLINE static __INLINE q31_t pid_q31(
    pid_instance_q31 *S,
    q31_t in)
{
    q63_t acc;
    q31_t out;

    /* acc = A0 * x[n]  */
    acc = (q63_t)S->A0 * in;

    /* acc += A1 * x[n-1] */
    acc += (q63_t)S->A1 * S->state[0];

    /* acc += A2 * x[n-2]  */
    acc += (q63_t)S->A2 * S->state[1];

    /* convert output to 1.31 format to add y[n-1] */
    out = (q31_t)(acc >> 31U);

    /* out += y[n-1] */
    out += S->state[2];

    /* Update state */
    S->state[1] = S->state[0];
    S->state[0] = in;
    S->state[2] = out;

    /* return to application */
    return (out);
}

#endif /*__PID_Q31_H */