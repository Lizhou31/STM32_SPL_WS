/***********************************************************************
* FILENAME :        PID_Q31.c
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
#include "PID_Q31.h"

/**
  @brief         Reset function for the Q31 PID Control.
  @param[in,out] S  points to an instance of the Q31 PID structure
  @return        none

  @par           Details
                   The function resets the state buffer to zeros.
 */

void pid_reset_q31(
    pid_instance_q31 *S)
{
    /* Reset state to zero, The size will be always 3 samples */
    memset(S->state, 0, 3U * sizeof(q31_t));
}

/**
  @brief         Initialization function for the Q31 PID Control.
  @param[in,out] S               points to an instance of the Q31 PID structure
  @param[in]     resetStateFlag
                   - value = 0: no change in state
                   - value = 1: reset state
  @return        none

  @par           Details
                   The <code>resetStateFlag</code> specifies whether to set state to zero or not. \n
                   The function computes the structure fields: <code>A0</code>, <code>A1</code> <code>A2</code>
                   using the proportional gain( \c Kp), integral gain( \c Ki) and derivative gain( \c Kd)
                   also sets the state variables to all zeros.
 */

void pid_init_q31(
    pid_instance_q31 *S,
    int32_t resetStateFlag)
{
    /* Derived coefficient A0 */
    S->A0 = add_q31(add_q31(S->Kp, S->Ki), S->Kd);

    /* Derived coefficient A1 */
    S->A1 = -add_q31(add_q31(S->Kd, S->Kd), S->Kp);

    /* Derived coefficient A2 */
    S->A2 = S->Kd;

    /* Check whether state needs reset or not */
    if (resetStateFlag)
    {
        /* Reset state to zero, The size will be always 3 samples */
        memset(S->state, 0, 3U * sizeof(q31_t));
    }
}