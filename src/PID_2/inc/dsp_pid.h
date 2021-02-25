#ifndef __DSP_PID_H
#define __DSP_PID_H

#include <dsp_math_type.h>
#include <string.h>

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

} pid_instance_q31, ARM_PID_INSTANCE_Q31[1];

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
CMSIS_INLINE static __INLINE q31_t arm_pid_q31(
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

#endif /* __DSP_PID_H */